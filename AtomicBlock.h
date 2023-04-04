#pragma once
#include<atomic>
#include<mutex>

#include<memory>

std::shared_ptr<int>a;

/*
* @brief 智能指针的原子计数块类
*/



class AtomicBlock
{
private:
	/***
	*	禁用复制构造函数和赋值运算符重载函数
	***/
	AtomicBlock(const AtomicBlock&) = delete;
	AtomicBlock& operator=(const AtomicBlock&) = delete;


	std::atomic<int> shared_count;  /*共享指针计数 = shared*/
	std::atomic<int> weak_count;  /*弱指针计数 = shared + weak*/



public:
	/*
	* @brief 构造函数
	*/
	AtomicBlock() 
		: shared_count(1)
		, weak_count(1){
		//std::cout<<"automic count block has constructed.\n";
	}

	virtual ~AtomicBlock(){
		//std::cout<< "automic count block has destroyed.\n";
	}

	//对引用计数进行原子加减
	void release_shared(){--shared_count;}
	void add_shared(){++shared_count;}
	void release_weak(){--weak_count;}
	void add_weak() {++weak_count;}

public:

	//获取引用计数
	int get_shared() const {return shared_count.load();}
	int get_weak() const {return weak_count.load();}
};