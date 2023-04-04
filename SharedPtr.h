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

	template<class Y> friend class SharedPtr;
    template<class Y> friend class WeakPtr;

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
	***/
	SharedPtr(const SharedPointer<T>& sptr)
		: _data(sptr._data)
		, _pcount(sptr._pcount) {
		add_SharedCount();
	}

	/*
	* @brief 移动构造函数
	* @param 
	*/
	SharedPointer(Shared_ptr&& uptr)
		: _data(uptr._data)
		, _pcount(uptr._pcount){
		uptr.reset();
	}

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
	/***
	*	@brief 赋值运算符重载函数
	*	@param sptr => 共享指针
	* 
	*	这里是线程不安全的主要因素，因为对原来对象的计数减一，与对新对象的计数加一，这两个操作合在一起不是原子操作
	***/
	SharedPointer& operator=(const SharedPointer& sptr) {
		// 修改指向需要竞争自身锁，这里不加锁是导致线程不安全的原因
		//std::unique_lock<std::mutex> self_guard(self_mutex);
		if (_data != sptr._data) {
			// 减少计数引用
			dec_SharedCount();
			// 指向新资源
			_data = sptr._data;
			_pcount = sptr._pcount;
			// 增加计数引用
			add_SharedCount();
		}
		return *this;
	}


	shared_ptr& operator=(shared_ptr&& sp) {
		if (_data != sptr._data) {
			dec_SharedCount();
			_data = sptr._data;
			_pcount = sptr._pcount;
			add_SharedCount();
		}
		sp.reset();
		return *this;
	}


	T* get() const noexcept {
		return data_;
	}
	/*
	* @brief *运算符重载函数
	*/
	T& operator*() const {
		return *_data;
	}

	/*
	* @brief ->运算符重载函数
	*/
	T* operator->() const {
		return _data;
	}

public:

	void reset() {
		dec_SharedCount();
		_data = nullptr;
		_pcount = nullptr;
	}

	void reset(T* data) {
		dec_shared_count();
		data_ = data;
		cblock_ = new cblock();
	}

	/*
	* @brief 获取共享引用数
	*/
	int use_count() {
		return _pcount->get_shared_count();
	}

	/*
	* @brief 判断是否为空
	*/
	bool is_empty() const {
		return (_data == nullptr);
	}
};

