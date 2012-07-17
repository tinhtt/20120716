#pragma once
#include "../VCommon/VCommonDefines.h"

namespace boost
{
	namespace asio
	{
#pragma pack(push, 1)
		struct Header
		{
			boost::uint32_t nLen;
			boost::uint32_t nCrc;
			boost::uint32_t nSeq;
			boost::uint16_t nType;
		};
#pragma pack(pop)

		class session;
		typedef session*			session_ptr;
		typedef VSharedPtr<session> session_ref;
		
		class handler_allocator
			: private boost::noncopyable
		{
		public:
			handler_allocator()
				: in_use_(false)
			{}

			void* allocate(const std::size_t& nSize)
			{
				if (!in_use_ && nSize < storage_.size)
				{
					in_use_ = true;
					return storage_.address();
				}
				else
				{
					return VAllocateManager::Instance().Allocate(nSize);
				}
			}

			void deallocate(void* pointer, const std::size_t& nSize)
			{
				if (pointer == storage_.address())
				{
					in_use_ = false;
				}
				else
				{
					VAllocateManager::Instance().DeAllocate(pointer, nSize);
				}
			}

		private:
			// Storage space used for handler-based custom memory allocation.
			boost::aligned_storage<1024> storage_;

			// Whether the handler-based custom allocation storage has been used.
			bool in_use_;
		};

		// Wrapper class template for handler objects to allow handler memory
		// allocation to be customised. Calls to operator() are forwarded to the
		// encapsulated handler.
		template <typename Handler>
		class session_alloc_handler
		{
		public:
			session_alloc_handler(handler_allocator& a, Handler h)
				: allocator_(a)
				, handler_(h)
			{}

			template <typename Arg1>
			void operator()(Arg1 arg1) {
				handler_(arg1);
			}

			template <typename Arg1, typename Arg2>
			void operator()(Arg1 arg1, Arg2 arg2) {
				handler_(arg1, arg2);
			}

			friend void* asio_handler_allocate(const std::size_t& nSize, session_alloc_handler<Handler>* this_handler) {
				return this_handler->allocator_.allocate(nSize);
			}

			friend void asio_handler_deallocate(void* pointer, const std::size_t nSize, session_alloc_handler<Handler>* this_handler) {
				this_handler->allocator_.deallocate(pointer, nSize);
			}

		private:
			handler_allocator& allocator_;
			Handler handler_;
		};

		// Helper function to wrap a handler object to add custom allocation.
		template <typename Handler>
		inline session_alloc_handler<Handler> make_session_alloc_handler(handler_allocator& a, Handler h) {
			return session_alloc_handler<Handler>(a, h);
		}

		class session
			: public boost::enable_shared_from_this<session>
		{		
			typedef VQueue<VSimpleBufferPtr> VSimpleBufferPtrContainer;
		public:
			session(io_service &io)
				: io_(io)
				, socket_(io)
				, strand_(io)
			{}
			
			~session() {}

		public:
			bool start(bool bflow = true)
			{
				try
				{
					VTcpSocket::non_blocking_io non_blocking_io(true);
					VTcp::no_delay option(true);
					socket_.set_option(option);
					socket_.io_control(non_blocking_io);
					
					while(!m_datas.empty()) {
						m_datas.pop();
					}				
				} catch(boost::system::system_error &) {
					return false;
				}

				if (bflow)
					read_flow();				
				else
					read_header();

				return true;
			}

			void reset()
			{
				if( UserRef user = user_.lock() )				
					user->CloseSession();
			}

			void close_socket()
			{
				boost::system::error_code error;
				user_.reset();
				socket_.close(error);						
			}

			VTcpSocket &socket() {
				return socket_;
			}

			template<typename T>
			void writeobject(const T &t) {
				write(&t, sizeof(T));
			}

			template<typename T1, typename T2>
			void writeobject(const T1 &t1, const T2 &t2) {
				write(&t1, sizeof(T1), &t2, sizeof(T2));
			}

			void write(const void *data, const size_t &l) {		
				sendbuffer(boost::make_shared<VSimpleBuffer>(data, l));				
			}

			void write(const void *data1, const size_t &l1, const void *data2, const size_t &l2) {		
				sendbuffer(boost::make_shared<VSimpleBuffer>(data1, l1, data2, l2));				
			}

			template<typename T>
			void send_packet(const T &t1, const T &t2) {
				write(t1.second, t1.first, t2.second, t2.first);
			}

			VString get_address()
			{
				const char *address_ = 0;
				try { address_ = socket_.remote_endpoint().address().to_string().c_str(); } catch(...) {}
				
				return VString(address_ ? address_ : "0.0.0.0");
			}

			void SetUser(UserRef user) {
				user_ = user;
			}

			void sendbuffer(const VSimpleBufferPtr &buffer)
			{
				if (buffer->isValid())				
					strand_.post(strand_.wrap(boost::bind(&session::do_write, this->shared_from_this(), boost::system::error_code(), buffer)));				
			}

		private:
			void do_write(const boost::system::error_code& e, VSimpleBufferPtr buffer)
			{
				if (!e)
				{					
					if (m_datas.empty())
					{
						m_datas.push(buffer);
						async_write(socket_
									, const_buffers_1(buffer->data_, buffer->l_)
									, make_session_alloc_handler(write_allocator_
																, strand_.wrap(boost::bind(&session::handle_write, this->shared_from_this(),_1))));
					}
					else
						m_datas.push(buffer);
				}
			}

			template<typename Handler>
			void read_data(void *data, const size_t &length, Handler handler)
			{
				async_read(socket_,buffer(data, length)
							, make_session_alloc_handler(read_allocator_, (handler)));
			}

			template<typename Handler>
			void write_data(const void *data, const size_t &length, Handler handler)
			{
				async_write(socket_,const_buffers_1(data, length)
							, make_session_alloc_handler(write_allocator_, (handler)));
			}

			void read_header() {
				read_data(&header_, sizeof(header_), boost::bind(&session::handle_read_header, shared_from_this(), _1));
			}

			void read_flow() {
				read_data(data_, 1, boost::bind(&session::handler_read_flow, shared_from_this(), _1));
			}
		private:
			void handler_read_flow(const boost::system::error_code& error)
			{
				if (!error)
				{
					switch(data_[0])
					{
					case '<':
						read_data( data_ + 1, header_policy_length, boost::bind(&session::handler_read_policy, shared_from_this(), _1));
						break;
					case 'A':
						read_header();
						break;
					default:
						break;
					}
				}
				else
					handler_error(error);
			}

			void handler_read_policy(const boost::system::error_code& error)
			{
				const char *xmlpolicy = "<cross-domain-policy><allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>";
				const char *requestpolicy = "<policy-file-request/>";
				if (!error)
				{
					if (std::memcmp(requestpolicy, data_, header_policy_length + 1) == 0)
						write_data(xmlpolicy, strlen(xmlpolicy) + 1, boost::bind(&session::handle_write_policy, shared_from_this(), _1));
				}
				else
					handler_error(error);
			}

			void handle_read_header(const boost::system::error_code& error)
			{
				if (!error && header_.nLen < max_length)
				{					
					if (header_.nLen > 0)
						read_data(data_, header_.nLen, boost::bind(&session::handle_read_body, shared_from_this(),_1));
					else
						handle_read_body(error);
				}
				else
					handler_error(error);
			}

			void handle_read_body(const boost::system::error_code& error)
			{
				if (!error)
				{
					//process data
					if (VNetwork::VProcessPacket::sProcessPacket)
					{
						ConstBuffer buff ( header_.nLen,data_) ;
						UserRef user = user_.lock();
						VNetwork::VProcessPacket::sProcessPacket->ReceivePacket(this, user.get(), &header_, buff);			
					}
					read_header();
				}
				else
					handler_error(error);
			}

			void handle_write(const boost::system::error_code& e)
			{
				if( !e )
				{
					m_datas.pop();
					if( !m_datas.empty() )
					{
						const VSimpleBufferPtr &buffer = m_datas.front();
						async_write(socket_,
							const_buffers_1(buffer->data_, buffer->l_),
							make_session_alloc_handler(write_allocator_,strand_.wrap(boost::bind(&session::handle_write, this->shared_from_this(),_1)))
							);
					}
				}
				else
					handler_error(e);

			}
			void handle_write_policy(const boost::system::error_code& error)
			{
				if( !error )
				{
					boost::system::error_code ignored_ec;
					socket_.shutdown(Ip::socket::shutdown_both, ignored_ec);
				}
				else
					handler_error(error);
				// nothing to do
			}
			void handler_error(const boost::system::error_code& error)
			{
				reset();
			}
		private:
			io_service &io_;
			Ip::socket socket_;	
			handler_allocator read_allocator_, write_allocator_;
			enum { 
				max_length = 1024,
				header_policy_length = 23 - 1,
			};
			char data_[max_length];	
			Header header_;
			VSimpleBufferPtrContainer m_datas;
			io_service::strand strand_;
			VString address_;
			UserWeakPtr user_;
		};		
		class session_manager
		{
			typedef VCommon::ObjectAllocator<session> session_allocator;
		public:	
			session_manager(io_service &io_, const size_t &max_session, const size_t &recycle_session) : io(io_), pool_session_(recycle_session)
			{				
			}
			session_ref create_session()
			{				
				return session_ref(create_session_ptr(), boost::bind(&session_manager::close_session, this, _1));
			}
			~ session_manager()
			{				
			}			
		protected:
			void clear_session()
			{
				pool_session_.clear(boost::bind(&session_allocator::Destroy, _1));				
			}
			virtual void start_accept() = 0;
		private:		
			session_ptr create_session_ptr()
			{
				session_ptr session_ptr_ = pool_session_.pop();				
				if( !session_ptr_ )
					session_ptr_ = session_allocator::Create<io_service>(io);
				return session_ptr_;
			}
			void close_session(session_ptr session)
			{
				if( session )
				{					
					session->reset();
					if( pool_session_.push(session) )
						session->close_socket();
					else
						session_allocator::Destroy(session);					
				}

			}
		private:
			io_service &io;												
			VCommon::SafeRingBuffer<session_ptr> pool_session_;

		};
		class async_server : public session_manager
		{
		public:
			async_server(int nthread, size_t max_session, size_t recycle_session, const unsigned short &port) : 
			  io(nthread),
				  acceptor_(io, Ip::endpoint(Ip::v4(), port)),
				  session_manager(io, max_session, recycle_session)
			  {				
				  start_accept();
				  for( size_t i = 0; i < nthread; ++i)
					  thread_.create_thread(boost::bind(&io_service::run, &io));				

			  }
			  virtual ~async_server(void)
			  {
			  }
			  void stop()
			  {
				  io.stop();
				  thread_.join_all();
				  clear_session();
			  }
		protected:
			void start_accept()
			{
				session_ref new_session = create_session();
				if( new_session )
				{
					acceptor_.async_accept(new_session->socket(),
						(boost::bind(&async_server::handle_accept, this, new_session,
						_1)));
				}
			}
		private:

			void handle_accept(session_ref new_session, const boost::system::error_code& error)
			{
				if( !error )				
					new_session->start();				
				else				
					new_session->socket().close();				
				start_accept();
			}

		private:
			io_service io;
			Ip::acceptor acceptor_;			
			boost::thread_group thread_;			
		};
	}
}
