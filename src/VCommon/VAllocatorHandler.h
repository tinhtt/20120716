#pragma once
#ifdef WIN32
	#define DEFAULT_SIZE_POOL 1000
#else
	#define DEFAULT_SIZE_POOL 32000
#endif

#define DEFAULT_SIZE_ALLOCATOR ( 32 << 10 )

namespace vcommon
{
	typedef boost::mutex		Mutex;
	typedef boost::shared_mutex	SharedMutex;
	
	typedef boost::mutex::scoped_lock		ScopedLock;
	typedef boost::shared_lock<SharedMutex>	SharedLock;
	typedef boost::unique_lock<SharedMutex>	UniqueLock;
	
	template<typename T>
	class SafeRingBuffer
	{	
		enum
		{
			init_value = 1,
			exchange_value = 0,
		};
		
	public:
		SafeRingBuffer() : cur_(0), size_(0), signal_(init_value)
		{}
		
		SafeRingBuffer(const size_t &size) : cur_(0), size_(size), signal_(0)
		{
			if (size_)
			{
				data_ = new T[size_];
				std::memset(data_, 0, size_ * sizeof(T));
			}
		}
		
		~SafeRingBuffer()
		{
			if (data_)
				delete []data_;
		}
		
		void size(const size_t &size)
		{
			if (size > 0)
			{
				size_ = size;
				data_ = new T[size_];
				std::memset(data_, 0, size_ * sizeof(T));
			}
		}
		
		bool push( const T &t)
		{
			if( (--signal_) == exchange_value )
			{
				if( cur_ < size_)	
				{
					data_[cur_++] = t;
					--signal_;
					return true;
				}
			}
			++signal_;
			return false;
		}
		
		T pop()
		{
			T t(0);
			if( (--signal_) == exchange_value )
			{
				if( cur_ > 0 )
					t = data_[--cur_];
			}
			++signal_;
			return t;
		}
		
		template<typename Handler>
		void clear(Handler handler) {
			std::for_each(data_, data_ + cur_, boost::bind(handler, _1));
		}
		
	private:
		T *data_;
		boost::detail::atomic_count signal_;
		size_t size_, cur_;
	};
	
	class handler_allocator : private boost::noncopyable
	{
	public:
		handler_allocator()
			: in_use_(false)
		{}

		void* allocate(std::size_t size)
		{
			if (!in_use_ && size < storage_.size)
			{
				in_use_ = true;
				return storage_.address();
			}
			else
			{
				return ::operator new(size);
			}
		}

		void deallocate(void* pointer)
		{
			if (pointer == storage_.address())
			{
				in_use_ = false;
			}
			else
			{
				::operator delete(pointer);
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
	class custom_alloc_handler
	{
	public:
		custom_alloc_handler(handler_allocator& a, Handler h)
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

		friend void* asio_handler_allocate(std::size_t size, custom_alloc_handler<Handler>* this_handler) {
			return this_handler->allocator_.allocate(size);
		}

		friend void asio_handler_deallocate(void* pointer, std::size_t /*size*/, custom_alloc_handler<Handler>* this_handler) {
			this_handler->allocator_.deallocate(pointer);
		}

		private:
			handler_allocator& allocator_;
			Handler handler_;
	};

	// Helper function to wrap a handler object to add custom allocation.
	template <typename Handler>
	inline custom_alloc_handler<Handler> make_custom_alloc_handler(handler_allocator& a, Handler h) {
		return custom_alloc_handler<Handler>(a, h);
	}
	
	class AllocateManager
	{
		typedef SafeRingBuffer<void *> AllocateSlot;
		
		enum 
		{
			max_slot = 16,
#ifdef WIN32
			max_thread = 4,
#else
			max_thread = 8,
#endif
		};
		
	private:
		AllocateManager() {}
		~AllocateManager() {}
		
	public:
		static AllocateManager &Instance()
		{
			static AllocateManager instance;
			return instance;
		}
		
		VUInt32 FindSlot(const VUInt32& nSize)
		{
			VUInt32 slot = 0;
			while (VUInt32(1 << slot) < nSize)
				++slot;
			
			return slot;
		}
		
		void* Allocate(const VUInt32& nSize)
		{			
			VUInt32 nSlot = FindSlot(nSize);
			void *pointer = 0;
			if (nSlot < max_slot)
			{
				for (size_t i = 0; i < max_thread; ++i)
					if (pointer = m_Container[nSlot][i].pop())
						break;
				
				if (!pointer)
					pointer = malloc(1 << nSlot);
			}
			else
				pointer = malloc(nSize);
				
			return pointer;
		}
		
		void DeAllocate(void *pointer, const VUInt32& nSize)
		{
			if (pointer)
			{
				VUInt32 nSlot = FindSlot(nSize);
				if (nSlot < max_slot)
					for (size_t i = 0; i < max_thread; ++i)
						if(m_Container[nSlot][i].push(pointer))
							return;
					
				free(pointer);
			}
		}
		
		void Stop()
		{
			for (size_t i = 0; i < max_slot; ++i) 
				for (size_t j = 0; j < max_thread; ++j)
					m_Container[i][j].clear(boost::bind(&free, _1));
		}
		
		void Start()
		{
			for (size_t i = 0; i < max_slot; ++i)
				for (size_t j = 0; j < max_thread; ++j)
					m_Container[i][j].size(DEFAULT_SIZE_POOL / max_thread);
		}
		
	private:
		AllocateSlot m_Container[max_slot][max_thread];
	};


	template<class _Ty>
	class ContainerAlloc
	{
	public:	
		typedef _Ty value_type;
		typedef value_type *pointer;
		typedef value_type& reference;
		typedef const value_type *const_pointer;
		typedef const value_type& const_reference;

		typedef size_t size_type;
		typedef ptrdiff_t difference_type;

		template<class _Other>
		struct rebind {	typedef ContainerAlloc<_Other> other; };

		pointer address(reference _Val) const { return (&_Val);	}
		const_pointer address(const_reference _Val) const { return (&_Val); }

		ContainerAlloc() throw() {}
		ContainerAlloc(const ContainerAlloc<_Ty>&) throw() {}

		template<class _Other>
		ContainerAlloc(const ContainerAlloc<_Other>&) throw() {}

		template<class _Other>
		ContainerAlloc<_Ty>& operator = (const ContainerAlloc<_Other>&) { return (*this); }

		void deallocate(pointer _Ptr, size_type _Count)
		{
			AllocateManager::Instance().DeAllocate((void*)_Ptr, (VUInt32)_Count * sizeof(_Ty));
		}

		pointer allocate(size_type _Count, const void* = 0)
		{
			return (pointer)AllocateManager::Instance().Allocate((VUInt32)_Count * sizeof(_Ty));
		}

		void construct(pointer _Ptr, const _Ty& _Val)
		{
			::new (_Ptr) _Ty(_Val);
		}

		void destroy(pointer _Ptr)
		{
			_Ptr->~_Ty();		
		}
	
		size_type max_size() const throw()
		{
			size_type _Count = (size_t)(-1) / sizeof (_Ty);
			return (0 < _Count ? _Count : 1);
		}
	};

	template <class T1, class T2>
	inline bool operator == (const ContainerAlloc<T1>&, const ContainerAlloc<T2>&) throw() { return true; }
	
	template <class T1, class T2>
	inline bool operator != (const ContainerAlloc<T1>&, const ContainerAlloc<T2>&) throw() { return false; }

	template<typename T>
	class Deque : public std::deque<T, ContainerAlloc<T> > {};
	
	template<typename T>
	class Vector : public std::vector<T, ContainerAlloc<T> > {};
	
	template<typename T>
	class List : public std::list<T, ContainerAlloc<T> > {};
	
	template<typename T>
	class Stack : public std::stack<T, Deque<T> > { };
	
	template<typename Key, typename T>
	class Map : public std::map<Key, T,std::less<Key> ,ContainerAlloc<std::pair<Key,T> > > {};
	
	template<typename T>
	class Set : public std::set<T,std::less<T>, ContainerAlloc<T> > {};
	
	template<typename T>
	class Queue : public std::queue<T, Deque<T> > {};
	
	template<typename T>
	class CircularBuffer : public boost::circular_buffer< T, ContainerAlloc<T> > {};
	
	//template<typename Key>
	//class SparseHashSet : public google::sparse_hash_set<Key, SPARSEHASH_HASH<Key>, std::equal_to<Key>, ContainerAlloc<Key> > {};
	//
	//template<typename Key>
	//class DenseHashSet : public google::dense_hash_set<Key, SPARSEHASH_HASH<Key>, std::equal_to<Key>, ContainerAlloc<Key> > {};
	//
	//template<typename Key, typename V>
	//class SparseHashMap : public google::sparse_hash_map <Key, V, SPARSEHASH_HASH<Key>, std::equal_to<Key>, ContainerAlloc<std::pair<Key, V> > > {};
	//
	//template<typename Key, typename V>
	//class DenseHashMap : public google::dense_hash_map <Key, V, SPARSEHASH_HASH<Key>, std::equal_to<Key>, ContainerAlloc<std::pair<Key, V> > > {};
	
	typedef std::basic_string<char, std::char_traits<char>, ContainerAlloc<char> >			String;
	typedef std::basic_string<char, std::char_traits<char>, ContainerAlloc<wchar_t> >		WString;
	typedef std::basic_stringstream<char, std::char_traits<char>, ContainerAlloc<char> >	StringStream;

};

