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
	* @brief ��ռָ�빹�캯��
	* @param uptr => ��ռָ��
	*/
	//SharedPointer(UniquePointer&& uptr);

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
	/*
	* @brief ��ֵ��������غ���
	* @param sptr => ����ָ��
	*/
	SharedPointer& operator=(const SharedPointer& sptr) {
		std::swap(*this, sp);
		// �޸�ָ����Ҫ����������
		std::unique_lock<std::mutex> self_guard(self_mutex);
		if (_pobject != sptr._pobject) {
			// ���ټ�������
			release_ref();
			// ָ������Դ
			_pobject = sptr._pobject;
			_pcount = sptr._pcount;
			// ���Ӽ�������
			add_ref();
		}
		return *this;
	}

	/*
	* @brief *��������غ���
	*/
	ObjectTest& operator*() const {
		return *_pobject;
	}

	/*
	* @brief ->��������غ���
	*/
	ObjectTest* operator->() const {
		return _pobject;
	}

public:
	/*
	* @brief ��ȡ����������
	*/
	int use_count() {
		// �鿴���ü���Ҳ��Ҫ����������
		std::unique_lock<std::mutex> self_guard(self_mutex);
		return _pcount->get_shared_count();
	}

	/*
	* @brief �ж��Ƿ�Ϊ��
	*/
	bool is_empty() const {
		return (_pobject == nullptr);
	}

	/*
	* @brief ��ȡ����������
	*/
	//void reset() {
	//	printf("reset starts.\n");
	//	// �޸�ָ����Ҫ����������
	//	std::unique_lock<std::mutex> self_guard(self_mutex);
	//	release_ref();
	//	_pobject = nullptr;
	//	_pcount = new AtomicBlock();
	//	printf("reset ends.\n");
	//}

};

