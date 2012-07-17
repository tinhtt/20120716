#pragma once

#include <iostream>
#include <algorithm>
#include <string>
#include <stack>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <fstream>
#include <ctime>
#include <queue>

//#include "google/sparsehash/sparseconfig.h"
//#include "google/sparse_hash_set"
//#include "google/sparse_hash_map"
//#include "google/dense_hash_set"
//#include "google/dense_hash_map"

#include "boost/cstdint.hpp"
#include "boost/date_time.hpp"
#include "boost/unordered_map.hpp"
#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include "boost/thread.hpp"
#include "boost/utility.hpp"
#include "boost/basic_repeating_timer.hpp"
#include "boost/smart_ptr/scoped_ptr.hpp"
#include "boost/smart_ptr/scoped_array.hpp"
#include "boost/smart_ptr/shared_ptr.hpp"
#include "boost/smart_ptr/shared_array.hpp"
#include "boost/smart_ptr/weak_ptr.hpp"
#include "boost/smart_ptr/intrusive_ptr.hpp"
#include "boost/enable_shared_from_this.hpp"
#include "boost/aligned_storage.hpp"
#include "boost/noncopyable.hpp"
#include "boost/circular_buffer.hpp"
#include "boost/detail/atomic_count.hpp"

typedef char							VInt8, VChar;
typedef short							VInt16;
typedef int								VInt32;
typedef unsigned char					VUInt8, VByte;
typedef unsigned short					VUInt16;
typedef unsigned int					VUInt32;
typedef bool							VBool;
typedef float							VFloat32;
typedef	double							VDouble64;

typedef boost::posix_time::ptime 		VTime;
typedef boost::asio::io_service			VBoostService;
typedef boost::system::error_code		VBoostError;
typedef boost::asio::ip::tcp			VTcp;
typedef boost::asio::ip::tcp::resolver	VResolver;
typedef boost::asio::ip::tcp::endpoint  VEndPoint;
typedef boost::asio::ip::tcp::socket	VTcpSocket;
typedef boost::asio::ip::tcp::acceptor  VAcceptor;
typedef boost::asio::deadline_timer		VDeadlineTimer;
typedef boost::asio::repeating_timer	VRepeatingTimer;
typedef boost::shared_mutex				VMutex;

#define VTrue				1
#define VFalse				0
#define VNull				0
#define VPair				std::pair
#define VPtrArray			boost::ptr_array
#define VSharedPtr			boost::shared_ptr
#define VSharedArrayPtr		boost::shared_array
#define VScopedPtr			boost::scoped_ptr
#define VScopedArrayPtr		boost::scoped_array
#define VWeakPtr			boost::weak_ptr
#define VIntrusivePtr		boost::intrusive_ptr
#define VHashTable			boost::unordered_map

static const VTime NullTime;
#define CHARS_IS_VALID(str) (str[sizeof(str) - 1] == 0)

struct VWriteLock : public boost::unique_lock<VMutex>
{
	typedef boost::unique_lock<VMutex> BaseClass;
	VWriteLock(VMutex &m)
		: BaseClass(m)
	{}
		
	VWriteLock(VMutex &m, const VInt32 &ms )
		: BaseClass(m, boost::posix_time::milliseconds(ms))
	{}
	
	void unlock()
	{
		if (owns_lock())
			boost::unique_lock<VMutex>::unlock();
	}
};

struct VReadLock : public boost::shared_lock<VMutex>
{
	typedef boost::shared_lock<VMutex> BaseClass;
	VReadLock(VMutex &m)
		: BaseClass(m)
	{}
	
	VReadLock(VMutex &m, const VInt32 &ms )
		: BaseClass(m, boost::get_system_time() +  boost::posix_time::milliseconds(ms))
	{}	
	
	void unlock()
	{
		if (owns_lock()) {
			boost::shared_lock<VMutex>::unlock();
		}
	}
};

struct VWriteLockTime : public VWriteLock
{
	VWriteLockTime(VMutex &m, const VInt32 &ms = 100) : VWriteLock(m, ms )
	{
	}
	
};

struct VReadLockTime : public VReadLock
{
	VReadLockTime(VMutex &m, const VInt32 &ms = 100) : VReadLock(m,ms )
	{
	}	
};

#include "VAllocatorHandler.h"
#include "VObjectAllocator.h"

typedef vcommon::String				VString;
typedef vcommon::WString			VWString;
typedef vcommon::StringStream		VStringStream;
typedef vcommon::AllocateManager	VAllocateManager;

#define VMap	vcommon::Map
#define VDeQue	vcommon::Deque
#define VVector	vcommon::Vector
#define VList	vcommon::List
#define VSet	vcommon::Set
#define VStack	vcommon::Stack
#define VQueue	vcommon::Queue

#define VObjectRefAllocator				vcommon::ObjectRefAllocator
#define VObjectIntrusivePtrAllocator	vcommon::ObjectIntrusivePtrAllocator
#define VSafeRingBuffer					vcommon::SafeRingBuffer
#define VCircularBuffer					vcommon::CircularBuffer

template<typename Key, typename T>
class VSafeMap : boost::noncopyable, public VMap<Key,T> {};

template<typename T>
class VSafeDeQue : boost::noncopyable, public VDeQue<T> {};

template<typename T>
class VSafeVector : boost::noncopyable, public VVector<T> {};

template<typename T>
class VSafeList : boost::noncopyable, VList<T> {};

template<typename T>
class VSafeSet : boost::noncopyable, public VSet<T> {};

template<typename T>
class VSafeCircularBuffer : boost::noncopyable, public VCircularBuffer<T> {};

template<typename Key>
class VSafeSparseHashSet : boost::noncopyable, public vcommon::SparseHashSet<Key> {};

template<typename Key>
class VSafeDenseHashSet : boost::noncopyable, public vcommon::DenseHashSet<Key> {};

template<typename Key, typename V>
class VSafeSparseHashMap : boost::noncopyable, public vcommon::SparseHashMap<Key, V> {};

template<typename Key, typename V>
class VSafeDenseHashMap : boost::noncopyable, public vcommon::DenseHashMap<Key, V> {};

template<typename T, const size_t N>
class VCircularStack
{
public:
	typedef T* iterator;
	typedef const T* const_iterator;

	VCircularStack()
		: index(0)
	{}
	
	virtual void push_back(const T &t) {
		datas[(index++) % N] = t;
	}
	
	virtual void clear() {
		index = 0;
	}
	
	virtual size_t get(T *data_, const bool &bReset = true)
	{
		size_t n = 0;
		if (index >= N)
		{
			index = index % N;
			std::copy(datas + index, datas + N, data_);
			if( index > 0)
				std::copy(datas, datas + index, data_ + (N - index));			
			n = N;
		} else if(index > 0 ) {
			std::copy(datas, datas + index, data_);			
			n = index;
		}
		
		if (bReset)
			index = 0;		
		return n;
	}
	
private:
	T datas[N];
	size_t index;	
};

template<typename T, const size_t N>
class VSafeCircularStack : public VCircularStack<T, N>
{
	typedef VCircularStack<T, N> BaseClass;
	
public:
	virtual void push_back(const T &t)
	{
		boost::unique_lock<boost::mutex> lock_(mutex_);
		BaseClass::push_back(t);
	}
	
	virtual void clear()
	{
		boost::unique_lock<boost::mutex> lock_(mutex_);
		BaseClass::clear();
	}
	
	virtual size_t get(T *data_, const bool &breset = true)
	{
		boost::unique_lock<boost::mutex> lock_(mutex_);
		return BaseClass::get(data_, breset);
	}
	
private:
	boost::mutex mutex_;
};

struct VSimpleBuffer
{
	void	*data_;
	size_t	len_;
	
	VSimpleBuffer(const void *data, const size_t &l)
	{
		if (l > 0 && data)
		{
			data_ = VAllocateManager::Instance().Allocate(l);
			std::memcpy(data_, data, l);
			len_ = l;
		}
		else
		{
			data_ = 0;
			len_ = 0;
		}
	}
	
	VSimpleBuffer(const void *data1, const size_t &l1,const void *data2, const size_t &l2)
	{
		size_t l = l1 + l2;
		if( l && (data1 || data2))
		{
			data_ = VAllocateManager::Instance().Allocate(l);
			if( data1 && l1)
				std::memcpy(data_, data1, l1);
			if( data2 && l2)
				std::memcpy((VChar*)data_ + l1, data2, l2);
			len_ = l;
		}
		else
		{
			data_ = 0;
			len_ = 0;
		}
	}

	~VSimpleBuffer()
	{
		if (isValid())
		{
			VAllocateManager::Instance().DeAllocate(data_, len_);
		}
	}
	
	const bool isValid() const
	{
		return data_ && len_;
	}
};
