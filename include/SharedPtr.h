#pragma once
#include "AtomicBlock.h"
#include <algorithm>
/*
* @brief 共享指针类
*/
template<class T>
class SharedPtr{
private:

	T* _data;
	AtomicBlock* _pcount;  /*指向原子计数代理块*/
	std::mutex self_mutex;  /*自身互斥量*/


private:
	/*
	* @brief 减少shared的引用计数
	*/
	void dec_SharedCount() {
		//线程不安全，为了线程安全，必要时应加锁
		//std::unique_lock<std::mutex> guard(self_mutex);
		if (!_pcount) return;
		_pcount->dec_shared();
		if (_pcount->get_shared() <= 0) {
			delete _data;
			delete _pcount;
			_data = nullptr;
			_pcount = nullptr;
		}
	}

	/***
	* @brief 增加对资源的引用
	***/
	void add_SharedCount() {
		if (!_pcount) return;
		_pcount->add_shared();
	}

public:
	/***
	* @brief 默认构造函数
	***/
	SharedPtr()
		: _data(nullptr)
		, _pcount(new AtomicBlock()){
	}

	/***
	*	@brief 指针构造函数
	*	@param 根据普通指针构造，可以直接接受new返回，默认为nullptr
	* 
	*	这里存在一个SharePtr的陷阱，即用同一个原始指针构造多个SharedPtr时，多个SharedPtr并不共享同一个Block
	*	而是会创造出多个Block，因此尽量不要这么做
	***/
	explicit SharedPtr(T* ptr)
		: _data(ptr)
		, _pcount(new AtomicBlock()){
	}

	/***
	*	@brief 复制构造函数
	*	@param SharedPtr的精髓，通过拷贝构造共享一个对象，并共享其所属block，将block中的sharecount+1；
	*
	* 
	* 
	***/
	SharedPtr(const SharedPointer<T>& sptr)
		: _data(sptr._data)
		, _pcount(sptr._pcount) {
		if()
		add_SharedCount();
	}

	/*
	* @brief 独占指针构造函数
	* @param uptr => 独占指针
	*/
	//SharedPointer(UniquePointer&& uptr);

	/*
	* @brief 弱指针构造函数
	* @param wptr => 弱指针
	*/
	SharedPointer(const WeakPointer<T>& wptr) 
		: _data(wptr._data)
		, _pcount(wptr._pcount){
		add_SharedCount();
	}

	/*
	* @brief 析构函数
	*/
	virtual ~SharedPointer() {
		// 减少计数引用
		dec_SharedCount();
	}



public:
	/*
	* @brief 赋值运算符重载函数
	* @param sptr => 共享指针
	*/
	SharedPointer& operator=(const SharedPointer& sptr) {
		std::swap(*this, sp);
		// 修改指向需要竞争自身锁
		std::unique_lock<std::mutex> self_guard(self_mutex);
		if (_pobject != sptr._pobject) {
			// 减少计数引用
			release_ref();
			// 指向新资源
			_pobject = sptr._pobject;
			_pcount = sptr._pcount;
			// 增加计数引用
			add_ref();
		}
		return *this;
	}

	/*
	* @brief *运算符重载函数
	*/
	ObjectTest& operator*() const {
		return *_pobject;
	}

	/*
	* @brief ->运算符重载函数
	*/
	ObjectTest* operator->() const {
		return _pobject;
	}

public:
	/*
	* @brief 获取共享引用数
	*/
	int use_count() {
		// 查看引用计数也需要竞争自身锁
		std::unique_lock<std::mutex> self_guard(self_mutex);
		return _pcount->get_shared_count();
	}

	/*
	* @brief 判断是否为空
	*/
	bool is_empty() const {
		return (_pobject == nullptr);
	}

	/*
	* @brief 获取共享引用数
	*/
	//void reset() {
	//	printf("reset starts.\n");
	//	// 修改指向需要竞争自身锁
	//	std::unique_lock<std::mutex> self_guard(self_mutex);
	//	release_ref();
	//	_pobject = nullptr;
	//	_pcount = new AtomicBlock();
	//	printf("reset ends.\n");
	//}

};

