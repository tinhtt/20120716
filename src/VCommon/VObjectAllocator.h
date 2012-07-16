#pragma once
namespace vcommon
{
	template<typename T, size_t max_pool = DEFAULT_SIZE_POOL, typename Allocator = AllocateManager>
	struct ObjectAllocator
	{
		typedef T value_type;
		typedef value_type *pointer;
		typedef ObjectAllocator<T, max_pool, Allocator> BaseClass;
		
		static pointer Create()
		{	
			pointer obj = (pointer)Allocator::Instance().Allocate(sizeof(T));
			if( obj )
				::new (obj)T();
			return obj;
		}
		
		template<typename T1>
		static pointer Create(T1 &t1)
		{	
			pointer obj = (pointer)Allocator::Instance().Allocate(sizeof(T));
			if( obj )
				::new (obj)T(t1);
			return obj;
		}
		
		template<typename T1,typename T2>
		static pointer Create(T1 &t1, T2 &t2)
		{	
			pointer obj = (pointer)Allocator::Instance().Allocate(sizeof(T));
			if( obj )
				::new (obj)T(t1, t2);
			return obj;
		}
		
		template<typename T1,typename T2,typename T3>
		static pointer Create(T1 &t1, T2 &t2, T3 &t3)
		{	
			pointer obj = (pointer)Allocator::Instance().Allocate(sizeof(T));
			if( obj )
				::new (obj)T(t1, t2, t3);
			return obj;
		}
		
		template<typename T1,typename T2,typename T3,typename T4>
		static pointer Create(T1 &t1, T2 &t2, T3 &t3, T4 &t4)
		{	
			pointer obj = (pointer)Allocator::Instance().Allocate(sizeof(T));
			if( obj )
				::new (obj)T(t1, t2, t3, t4);
			return obj;
		}
		
		template<typename T1,typename T2,typename T3,typename T4,typename T5>
		static pointer Create(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5)
		{	
			pointer obj = (pointer)Allocator::Instance().Allocate(sizeof(T));
			if( obj )
				::new (obj)T(t1, t2, t3, t4, t5);
			return obj;
		}
		
		static void Destroy(pointer obj)
		{
			if( obj )
			{
				obj->~T();
				Allocator::Instance().DeAllocate(obj, sizeof(T));
			}
		}
	};
	
	template<typename T, typename ObjectRef = SharedPtr<T>, size_t max_pool = DEFAULT_SIZE_POOL, typename Allocator = AllocateManager>
	struct ObjectRefAllocator
	{		
		typedef ObjectRefAllocator<T, ObjectRef, max_pool, Allocator> BaseClass;
		typedef ObjectAllocator<T, max_pool, Allocator> BaseAllocator;
		
		static ObjectRef Create()
		{
			//static BaseAllocator &instance = BaseAllocator::Instance();
			return ObjectRef((T*)BaseAllocator::Create(), boost::bind(&BaseClass::Destroy, _1));
		}
		
		template<typename T1>
		static ObjectRef Create(T1 &t1)
		{	
			return ObjectRef((T*)BaseAllocator::Create(t1), boost::bind(&BaseClass::Destroy, _1));
		}
		
		template<typename T1,typename T2>
		static ObjectRef Create(T1 &t1, T2 &t2)
		{
			return ObjectRef((T*)BaseAllocator::Create(t1, t2), boost::bind(&BaseClass::Destroy, _1));
		}
		
		template<typename T1,typename T2, typename T3>
		static ObjectRef Create(T1 &t1, T2 &t2, T3 &t3)
		{
			return ObjectRef((T*)BaseAllocator::Create(t1, t2, t3), boost::bind(&BaseClass::Destroy, _1));
		}
		
		template<typename T1,typename T2, typename T3, typename T4>
		static ObjectRef Create(T1 &t1, T2 &t2, T3 &t3, T4 &t4)
		{
			return ObjectRef((T*)BaseAllocator::Create(t1, t2, t3, t4), boost::bind(&BaseClass::Destroy, _1));
		}
		
		template<typename T1,typename T2,typename T3,typename T4,typename T5>
		static ObjectRef Create(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5)
		{
			return ObjectRef((T*)BaseAllocator::Create(t1, t2, t3, t4, t5), boost::bind(&BaseClass::Destroy, _1));
		}
		
		static void Destroy( T *t)
		{
			BaseAllocator::Destroy(t);
		}
	};
	
	template<typename T, typename ObjectRef = boost::intrusive_ptr<T>, size_t max_pool = DEFAULT_SIZE_POOL, typename Allocator = AllocateManager>
	struct ObjectIntrusivePtrAllocator
	{
		typedef ObjectRefAllocator<T, ObjectRef, max_pool, Allocator> BaseClass;
		typedef ObjectAllocator<T, max_pool, Allocator> BaseAllocator;
		
		static ObjectRef Create()
		{
			return ObjectRef((T*)BaseAllocator::Create());
		}
		
		template<typename T1>
		static ObjectRef Create(T1 &t1)
		{
			return ObjectRef((T*)BaseAllocator::Create(t1));
		}
		
		template<typename T1,typename T2>
		static ObjectRef Create(T1 &t1, T2 &t2)
		{
			return ObjectRef((T*)BaseAllocator::Create(t1, t2));
		}
		
		template<typename T1,typename T2, typename T3>
		static ObjectRef Create(T1 &t1, T2 &t2, T3 &t3)
		{
			return ObjectRef((T*)BaseAllocator::Create(t1, t2, t3));
		}
		
		template<typename T1,typename T2, typename T3, typename T4>
		static ObjectRef Create(T1 &t1, T2 &t2, T3 &t3, T4 &t4)
		{
			return ObjectRef((T*)BaseAllocator::Create(t1, t2, t3, t4));
		}
		
		template<typename T1,typename T2,typename T3,typename T4,typename T5>
		static ObjectRef Create(T1 &t1, T2 &t2, T3 &t3, T4 &t4, T5 &t5)
		{
			return ObjectRef((T*)BaseAllocator::Create(t1, t2, t3, t4, t5));
		}
	};
};

#ifndef  _LIB_LOG
namespace boost
{
	template<typename T>
	inline void intrusive_ptr_add_ref(T* expr) {
		expr->AddRef();
	}
	 
	template<typename T>
	inline void intrusive_ptr_release(T* expr) {
		if(expr->DecreaseRef())
			VCommon::ObjectIntrusivePtrAllocator<T>::BaseAllocator::Destroy(expr);
	}
};
#endif