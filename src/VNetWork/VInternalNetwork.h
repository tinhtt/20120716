#pragma once
#include "VCommonDefines.h"

namespace boost
{
	namespace asio
	{		
		namespace VIntenalNetWork
		{
			typedef ip::tcp Ip;			
			typedef boost::shared_ptr<io_service> io_ref;	
			typedef boost::function<void (void*)> handler_func_type;
			typedef boost::shared_ptr<deadline_timer> timer_ref;
			class VAsyncIo
			{
			public:
				VAsyncIo() : workref( new io_service::work(io_)) , threadref( new boost::thread(boost::bind(&io_service::run, &io_)))//, timer_(io_)
				{
					//;
				}
				~VAsyncIo()
				{
				}
				static VAsyncIo &Instance()
				{
					static VAsyncIo instance;
					return instance;
				}
				void Stop()
				{
					io_.stop();
					//timer_.cancel();//
					if( workref)
						workref.reset();
					if( threadref)
					{
						threadref->join();
						threadref.reset();
					}
				}
				io_service &get()
				{
					return io_;
				}
				template<typename Handler>
				timer_ref TimerCallBack(const VInt32 &ms, const Handler &handler_)
				{
					timer_ref timer_( new deadline_timer(io_));
					timer_->expires_from_now(boost::posix_time:: milliseconds(ms));
					timer_->async_wait(handler_);
					return timer_;
				}
			private:
				
				io_service io_;
				boost::shared_ptr<io_service::work> workref;
				boost::shared_ptr<boost::thread> threadref;
				//deadline_timer timer_;//(io_);
				
			};
			template<typename Header, typename ProcessPacket>
			class connection : public boost::enable_shared_from_this<connection<Header, ProcessPacket> >
			{
				typedef std::vector<char> msg_container;
				typedef boost::shared_ptr<msg_container> msg_ref;
				typedef std::list<msg_ref> msg_container_ref;				
			public:
				connection() : io_(VAsyncIo::Instance().get()), socket_(io_), stopped_(true), strand_(io_) 
				{
				}
				virtual ~connection()
				{
				}
				void close()
				{
					if (socket_.is_open())
						socket_.close();
				}
				virtual void start()
				{
					stopped_ = false;
					read_header();
				}
				virtual void OnError(const boost::system::error_code& e)
				{
					stopped_ = true;
					if( !e )
						std::cout << "OnError : " << e.message() << std::endl;
				}
				virtual bool OnPacket()
				{
					return ProcessPacket::Instance().ProcessPacket(this, &header_, read_msg_.empty() ? 0 : &read_msg_[0]);
				}
				void write_buffer(const msg_ref &cont_, const void *data, const VUInt32 &l)
				{
					cont_->insert(cont_->end(), (const char *)data, (const char *)data + l);
				}
				template<typename T>
				void write_buffer(const msg_ref &cont_, const T &t)
				{
					write_buffer(cont_, &t, sizeof(T));					
				}				
				template<typename T>
				void write(const boost::uint16_t &type, const T &t)
				{
					write(type, &t, sizeof(T));
				}
				template<typename T>
				void send_packet(const boost::uint16_t &type, const T &t)
				{
					write(type, t);
				}
				template<typename T>
				void SendPacket(const boost::uint16_t &type, const T &t)
				{
					write(type, t);
				}
				void SendPacket(const boost::uint16_t &type, const void *data, const VUInt32 &l)
				{
					write(type, data, l);
				}
				template<typename Header_, typename T>
				void SendPacketHeader(const Header_ &header, const T &t)
				{
					msg_ref msg(new msg_container);
					write_buffer(msg, header);
					write_buffer(msg, t);
					strand_.post(strand_.wrap(boost::bind(&connection::do_write, this->shared_from_this(), msg)));
				}
				template<typename T>
				void write(const T &t)
				{
					msg_ref msg(new msg_container);
					write_buffer(msg, t);
					strand_.post(strand_.wrap(boost::bind(&connection::do_write, this->shared_from_this(), msg)));
				}
				void write(const boost::uint16_t &type, const void *data, const VUInt32 &l)
				{
					if( data && l )
					{						
						Header header = {l, type};
						msg_ref msg(new msg_container);
						write_buffer(msg, header);
						write_buffer(msg, data, l);
						strand_.post(strand_.wrap(boost::bind(&connection::do_write, this->shared_from_this(), msg)));
					}
				}
				Ip::socket &socket()
				{
					return socket_;
				}
				template<typename T>
				boost::shared_ptr<T> get_shared_ptr()
				{
					return boost::dynamic_pointer_cast<T>(this->shared_from_this());
				}
			protected:				
				void read_header()
				{
					read_data(&header_, sizeof(header_), boost::bind(&connection::handle_read_header, this->shared_from_this(), _1));
				}				
				void read_body()
				{
					read_data(&read_msg_[0], read_msg_.size(), boost::bind(&connection::handle_read_body, this->shared_from_this(), _1));
				}
			private:				
				void do_write(msg_ref msg)
				{
					if( stopped_ == true )
						return;
					write_msg_.push_back(msg);
					if( write_msg_.size() == 1 )// write
					{
						async_write(socket_,
							boost::asio::buffer(*write_msg_.front()),
							strand_.wrap(boost::bind(&connection::handle_write, this->shared_from_this(),
							_1)));
					}					
				}				
				void handle_write(const boost::system::error_code& error)
				{
					if( !error )
					{
						write_msg_.pop_front();
						if( !write_msg_.empty() )
						{
							async_write(socket_,
								boost::asio::buffer(*write_msg_.front()),
								strand_.wrap(boost::bind(&connection::handle_write, this->shared_from_this(),
								_1)));
						}

					}
					else
						OnError(error);
				}				
				void handle_read_header(const boost::system::error_code& error)
				{
					if( !error )
					{
						if( header_.nLen )
						{
							read_msg_.resize(header_.nLen);
							read_body();
						}
						else
						{
							OnPacket();
							read_header();
						}
					}
					else
						OnError(error);
				}				
				void handle_read_body(const boost::system::error_code& error)
				{
					if( !error )
					{
						OnPacket();
						read_header();
					}
					else
						OnError(error);
				}
				template<typename Handler>
				void read_data(void *data, const VUInt32 &length, Handler handler)
				{
					async_read(socket_,buffer(data, length),handler);
				}				
			protected:
				Header header_;
				msg_container read_msg_;
				msg_container_ref write_msg_;
				io_service &io_;
				Ip::socket socket_;		
				bool stopped_;
				io_service::strand strand_;	

			};				
			template<typename T>		
			static void create_client(const char *addr, const char *port, handler_func_type connect_, handler_func_type close_)
			{
				//io_ref io( new io_service);
				boost::shared_ptr<T> client_(new T(connect_, close_));
				client_->connect(addr, port);						
			}
			template<typename Header,typename ProcessPacket>
			class client : 
				public connection<Header, ProcessPacket>
			{		
			public:				
				typedef boost::shared_ptr<client<Header, ProcessPacket> > client_ref;							
				typedef connection<Header, ProcessPacket> BaseClass;
			public:
				client(handler_func_type connect_ = 0, handler_func_type close_ = 0) : BaseClass(), connect_handler_(connect_), close_handler_(close_)
				{
				}
				~client()
				{
					/*if( close_handler_ )
						close_handler_(this);
					std::cout << "Destructor client" << std::endl;
					*/
				}	
				virtual void OnError(const boost::system::error_code& e)
				{
					BaseClass::OnError(e);
					if( e != boost::asio::error::operation_aborted && e && close_handler_ )
						close_handler_(this);
				}
				void connect(const char *addr, const char *port)
				{					
					 Ip::resolver resolver(BaseClass::io_);
					 Ip::resolver::query query(addr, port);
					 do_connect(resolver.resolve(query));	
					 /*io_service &io = BaseClass::io_;
					 thread_ref_.reset(new boost::thread(boost::bind(&io_service::run, &io)));	*/
				}	
				
				void stop()
				{
					/*if( io_ref_ )
						io_ref_->stop();
					if( thread_ref_ )
					{
						thread_ref_->join();
						thread_ref_.reset();
					}*/
				}
			private:				
				void do_connect(Ip::resolver::iterator iterator)
				{
					ip::tcp::endpoint endpoint = *iterator;
					boost::shared_ptr<client> client_ref = boost::dynamic_pointer_cast<client>(BaseClass::shared_from_this());//this->get_shared_ptr<client>();
					this->socket_.async_connect(endpoint, boost::bind(&client::handle_connect, client_ref,_1, ++iterator));
				}								
				void handle_connect(const boost::system::error_code& error,
					Ip::resolver::iterator endpoint_iterator)
				{
					if( !error )
					{
						this->start();
						if( connect_handler_ )
							connect_handler_(this);
					}
					else if( endpoint_iterator != Ip::resolver::iterator())
					{
						do_connect(endpoint_iterator);
					}
					else
					{
						std::cout << "Connect error : " << error.message() << std::endl;
					}

				}
			private:
				/*boost::shared_ptr<boost::thread> thread_ref_;
				io_ref io_ref_;*/
				handler_func_type connect_handler_;
				handler_func_type close_handler_;				
			};
			template<typename session>
			class server
			{			
			public:
				typedef boost::shared_ptr<session> session_ref;
				typedef session * session_ptr;
				typedef VCommon::ObjectAllocator<session> session_allocator;
			public:
				server(const unsigned short &port) :
				  acceptor_(VAsyncIo::Instance().get(), Ip::endpoint(Ip::v4(), port))
				{
					start_accept();
					//boost::shared_ptr<boost::thread> thread_ref_( new boost::thread(boost::bind(&io_service::run, &io)));										
				}
				server() : acceptor_(VAsyncIo::Instance().get())
				{
				}				
				~server()
				{
				}
				virtual void stop_session(session_ptr session_)
				{
					std::cout << "Stop Connection" << std::endl;
				}
				virtual void open_session(session_ptr session_)
				{
					std::cout << "Open Connection" << std::endl;
				}
				void open(const unsigned short &port)
				{
					Ip::endpoint endpoint(Ip::v4(), port);
					acceptor_.open(endpoint.protocol());
					acceptor_.bind(endpoint);
					acceptor_.listen();
					start_accept();
					//boost::shared_ptr<boost::thread> thread_ref_( new boost::thread(boost::bind(&io_service::run, &io)));

				}
				void stop()
				{
					/*io.stop();
					if( thread_ref_ )
					{
						thread_ref_->join();
						thread_ref_.reset();
					}
					*/
				}
			private:
				void start_accept()
				{
					session_ref new_session = create_session();
					if( new_session )
					{
						acceptor_.async_accept(new_session->socket(),
							boost::bind(&server::handle_accept, this, new_session,_1));
					}
				}			
				void handle_accept(session_ref new_session, const boost::system::error_code& error)
				{
					if( !error )
					{
						new_session->start();
						open_session(new_session.get());
					}			
					else
					{
						std::cout << "handle_accept error: " << error.message() << std::endl;
					}
					start_accept();
				}
			private:
				session_ref create_session()
				{
					return session_ref(session_allocator::Create(), boost::bind(&server::destroy_session, this, _1));
				}
				void destroy_session(session_ptr session_)
				{
					stop_session(session_);
					session_allocator::Destroy(session_);
				}
			private:
				//io_service io;
				Ip::acceptor acceptor_;
				//boost::shared_ptr<boost::thread> thread_ref_;
			};			
			template<typename session>
			class client_manager 
			{				
			public:
				template <typename Type, typename T >
				bool SendPacket(const Type &type, const T &t)
				{
					if(VSharedPtr<session> client = client_.lock())
					{
						client->write(type, t);					
						return true;
					}
					return false;
				}
				template <typename Type>
				bool SendPacket(const Type &type, const void *data, const VUInt32 &l)
				{
					if(VSharedPtr<session> client = client_.lock())
					{
						client->write(type, data, l);
						return true;
					}
					return false;
				}
				void Connect(const char *pszAdd, const char *pszPort)
				{
					create_client<session>(pszAdd, pszPort, boost::bind(&client_manager::connect, this, _1),boost::bind(&client_manager::close, this, _1));
				}
				static client_manager &Instance()
				{
					static client_manager instance;
					return instance;
				}
			private:
				void connect(void *conn)
				{
					client_ = boost::dynamic_pointer_cast<session>((static_cast<session*>(conn))->shared_from_this());//((session *)conn)->get_shared_ptr<session>();
				}
				void close(void *conn)
				{
					/*if( client_.lock().get() == conn )
						client_ = 0;
						*/
				}
			protected:
				VWeakPtr<session> client_;
				//session *client_;
			};
			template<typename session>
			class multi_client_manager
			{				
			public:	
				typedef typename std::list<session*>::iterator SessionIt;			
				void Connect(const char *pszAdd, const char *pszPort)
				{
					create_client<session>(pszAdd, pszPort, boost::bind(&multi_client_manager::connect, this, _1),boost::bind(&multi_client_manager::close, this, _1));
				}
				~multi_client_manager()
				{
					/*VWriteLock lock(mutex_);
					std::for_each(client_.begin(), client_.end(), boost::bind(&session::stop, _1));
					client_.clear();*/
				}
				/*static multi_client_manager<session> &Instance()
				{
					static multi_client_manager<session> instance;
					return instance;
				}*/
			private:
				void connect(void *conn)
				{
					VWriteLock lock(mutex_);
					client_.push_back(boost::dynamic_pointer_cast<session>((static_cast<session*>(conn))->shared_from_this()));
					//client_.push_back((static_cast<session*>(conn))->get_shared_ptr<session>());
				}
				void close(void *conn)
				{
					/*
					VWriteLock lock(mutex_);
					SessionIt it = std::find(client_.begin(), client_.end(), static_cast<session*>(conn));
					if( it != client_.end() )
						client_.erase(it);
						*/
				}
			protected:
				std::list<VWeakPtr<session> > client_;
				VMutex mutex_;
			};
		};

	};
};
