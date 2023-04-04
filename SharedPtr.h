#pragma once
#include "AtomicBlock.h"
#include <algorithm>
/*
* @brief ����ָ����
*/
template<class T>
class SharedPtr{
private:

	T* _data;
	AtomicBlock* _pcount;  /*ָ��ԭ�Ӽ��������*/
	std::mutex self_mutex;  /*��������*/


private:
	/*
	* @brief ����shared�����ü���
	*/
	void dec_SharedCount() {
		//�̲߳���ȫ��Ϊ���̰߳�ȫ����ҪʱӦ����
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
	* @brief ���Ӷ���Դ������
	***/
	void add_SharedCount() {
		if (!_pcount) return;
		_pcount->add_shared();
	}

	template<class Y> friend class SharedPtr;
    template<class Y> friend class WeakPtr;

public:
	/***
	* @brief Ĭ�Ϲ��캯��
	***/
	SharedPtr()
		: _data(nullptr)
		, _pcount(new AtomicBlock()){
	}

	/***
	*	@brief ָ�빹�캯��
	*	@param ������ָͨ�빹�죬����ֱ�ӽ���new���أ�Ĭ��Ϊnullptr
	* 
	*	�������һ��SharePtr�����壬����ͬһ��ԭʼָ�빹����SharedPtrʱ�����SharedPtr��������ͬһ��Block
	*	���ǻᴴ������Block����˾�����Ҫ��ô��
	***/
	explicit SharedPtr(T* ptr)
		: _data(ptr)
		, _pcount(new AtomicBlock()){
	}

	/***
	*	@brief ���ƹ��캯��
	*	@param SharedPtr�ľ��裬ͨ���������칲��һ�����󣬲�����������block����block�е�sharecount+1��
	***/
	SharedPtr(const SharedPointer<T>& sptr)
		: _data(sptr._data)
		, _pcount(sptr._pcount) {
		add_SharedCount();
	}

	/*
	* @brief �ƶ����캯��
	* @param 
	*/
	SharedPointer(Shared_ptr&& uptr)
		: _data(uptr._data)
		, _pcount(uptr._pcount){
		uptr.reset();
	}

	/*
	* @brief ��ָ�빹�캯��
	* @param wptr => ��ָ��
	*/
	SharedPointer(const WeakPointer<T>& wptr) 
		: _data(wptr._data)
		, _pcount(wptr._pcount){
		add_SharedCount();
	}

	/*
	* @brief ��������
	*/
	virtual ~SharedPointer() {
		// ���ټ�������
		dec_SharedCount();
	}



public:
	/***
	*	@brief ��ֵ��������غ���
	*	@param sptr => ����ָ��
	* 
	*	�������̲߳���ȫ����Ҫ���أ���Ϊ��ԭ������ļ�����һ������¶���ļ�����һ����������������һ����ԭ�Ӳ���
	***/
	SharedPointer& operator=(const SharedPointer& sptr) {
		// �޸�ָ����Ҫ���������������ﲻ�����ǵ����̲߳���ȫ��ԭ��
		//std::unique_lock<std::mutex> self_guard(self_mutex);
		if (_data != sptr._data) {
			// ���ټ�������
			dec_SharedCount();
			// ָ������Դ
			_data = sptr._data;
			_pcount = sptr._pcount;
			// ���Ӽ�������
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
	* @brief *��������غ���
	*/
	T& operator*() const {
		return *_data;
	}

	/*
	* @brief ->��������غ���
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
	* @brief ��ȡ����������
	*/
	int use_count() {
		return _pcount->get_shared_count();
	}

	/*
	* @brief �ж��Ƿ�Ϊ��
	*/
	bool is_empty() const {
		return (_data == nullptr);
	}
};

