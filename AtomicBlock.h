#pragma once
#include<atomic>
#include<mutex>

#include<memory>

std::shared_ptr<int>a;

/*
* @brief ����ָ���ԭ�Ӽ�������
*/



class AtomicBlock
{
private:
	/***
	*	���ø��ƹ��캯���͸�ֵ��������غ���
	***/
	AtomicBlock(const AtomicBlock&) = delete;
	AtomicBlock& operator=(const AtomicBlock&) = delete;


	std::atomic<int> shared_count;  /*����ָ����� = shared*/
	std::atomic<int> weak_count;  /*��ָ����� = shared + weak*/



public:
	/*
	* @brief ���캯��
	*/
	AtomicBlock() 
		: shared_count(1)
		, weak_count(1){
		//std::cout<<"automic count block has constructed.\n";
	}

	virtual ~AtomicBlock(){
		//std::cout<< "automic count block has destroyed.\n";
	}

	//�����ü�������ԭ�ӼӼ�
	void release_shared(){--shared_count;}
	void add_shared(){++shared_count;}
	void release_weak(){--weak_count;}
	void add_weak() {++weak_count;}

public:

	//��ȡ���ü���
	int get_shared() const {return shared_count.load();}
	int get_weak() const {return weak_count.load();}
};