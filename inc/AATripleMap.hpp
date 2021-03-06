﻿/*  
 * Copyright (c) 2015 ArmyAnt
 * 版权所有 (c) 2015 ArmyAnt
 *
 * Licensed under the BSD License, Version 2.0 (the License); 
 * 本软件使用BSD协议保护, 协议版本:2.0
 * you may not use this file except in compliance with the License. 
 * 使用本开源代码文件的内容, 视为同意协议
 * You can read the license content in the file "LICENSE" at the root of this project
 * 您可以在本项目的根目录找到名为"LICENSE"的文件, 来阅读协议内容
 * You may also obtain a copy of the License at 
 * 您也可以在此处获得协议的副本:
 * 
 *     http://opensource.org/licenses/BSD-3-Clause
 * 
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
 * 除非法律要求或者版权所有者书面同意,本软件在本协议基础上的发布没有任何形式的条件和担保,无论明示的或默许的.
 * See the License for the specific language governing permissions and limitations under the License. 
 * 请在特定限制或语言管理权限下阅读协议
 */

#ifndef ARMY_ANT_TRIPLE_MAP_2015_11_19
#define ARMY_ANT_TRIPLE_MAP_2015_11_19

/*	* @ author			: Jason
	* @ date			: 11/21/2015
	* @ last update		: 12/18/2015
	* @ summary			: 三元组及具有双值的键值map
	* @ uncompleted		:
	* @ untested		: all 已经在使用中,但不保证没有bug
	* @ issue			:
	*/

#include <cstddef>
#include <functional>
#include <vector>
#include "AADefine.h"

namespace ArmyAnt {

template <class _Key, class _Value1, class _Value2>
class TripleMap;
template <class _Key, class _Value1, class _Value2>
class Iterator_TripleMap;

// 三元组
template <class _First, class _Second, class _Third>
class Triad
{
public:
	typedef Triad<_First, _Second, _Third> SelfType;
	//三元表的迭代器
	typedef Iterator_TripleMap<_First, _Second, _Third> Iterator;

public:
	//默认空构造函数
	Triad();
	//带值构造函数
	Triad(const _First&, const  _Second&, const  _Third&);
	//复制构造函数
	Triad(const Triad&);
	//从迭代器复制值的构造函数
	Triad(const Iterator&);
	//拷贝赋值
	Triad&operator=(const SelfType&);
	//从迭代器拷贝赋值
	Triad&operator=(const Iterator&);
	//析构函数
	~Triad();

public:
	//判断是否相等
	bool Equals(const SelfType&) const;
	//交换内容12
	inline Triad<_Second, _First, _Third> Swap12() const;
	//交换内容13
	inline Triad<_Third, _Second, _First> Swap13() const;
	//交换内容23
	inline Triad<_First, _Third, _Second> Swap23() const;
	//获取二元组12
	inline std::pair<_First, _Second> GetValue12() const;
	//获取二元组13
	inline std::pair<_First, _Third> GetValue13() const;
	//获取二元组23
	inline std::pair<_Second, _Third> GetValue23() const;

public:
	inline bool operator == (const SelfType&) const;
	inline bool operator != (const SelfType&) const;
	inline bool operator == (const Iterator&) const;
	inline bool operator != (const Iterator&) const;

public:
	_First first;
	_Second second;
	_Third third;
};

template <class _Key, class _Value1, class _Value2>
class TripleMap
{
public:
	typedef TripleMap<_Key, _Value1, _Value2> SelfMap;
	typedef Iterator_TripleMap<_Key, _Value1, _Value2> Iterator;
	typedef Triad<_Key, _Value1, _Value2> Element;

public:
	TripleMap(Triad<_Key, _Value1, _Value2>* dataArray = nullptr, uint32 num = 0);
	~TripleMap();

public:
	//判断是否相等
	bool Equals(const SelfMap&) const;
	//判断是否为空
	inline bool Empty() const;
	//获取对应键处的值
	std::pair<_Value1, _Value2> GetValues(const _Key&) const;
	//获取数据总量
	uint32 Size() const;

	//插入键值
	bool Insert(const _Key&, const  _Value1&, const  _Value2&);
	//插入迭代器所指内容
	bool Insert(const Iterator&);
	//插入元素
	inline bool Insert(const Element&);
	//插入键值
	inline bool Insert(const _Key&, const std::pair<_Value1, _Value2>&);
	//清除迭代器所指位置的内容
	bool Erase(Iterator&);
	//清除键所对应处的内容
	bool Erase(const _Key&);

	//查找指定键的位置
	Iterator Find(const _Key&);
	const Iterator Find(const _Key&) const;
	//获取指向开头的迭代器
	Iterator Begin();
	const Iterator Begin() const;
	//令迭代器向前指
	Iterator&Before(const Iterator&, int num = 1);
	//令迭代器向后指
	Iterator&After(const Iterator&, int num = 1);
	//迭代器末尾,即空值
	inline static const Iterator&End();

	//排序算法
	typedef std::function<bool(const Iterator&, const Iterator&)> SortFunc;
	//按指定算法进行排序
	void Sort(SortFunc);
	//清空三元表
	bool Clear();
	//清空所有值,但保留键
	bool ClearValues();

public:
	inline bool operator ==(const SelfMap&) const;
	inline bool operator !=(const SelfMap&) const;
	inline bool operator ==(std::nullptr_t) const;
	inline bool operator !=(std::nullptr_t) const;
	inline operator bool() const;
	inline bool operator !() const;
	inline Iterator operator[] (const _Key&);
	inline const Iterator operator[] (const _Key&) const;
	inline const Iterator operator[] (std::nullptr_t);

public:
	std::vector<Triad<_Key, _Value1, _Value2>> datas;
};

template <class _Key, class _Value1, class _Value2>
class Iterator_TripleMap
{
public:
	typedef Iterator_TripleMap<_Key, _Value1, _Value2> Iterator;
	typedef TripleMap<_Key, _Value1, _Value2> SelfMap;
	typedef Triad<_Key, _Value1, _Value2> Element;

public:
	Iterator_TripleMap(TripleMap<_Key, _Value1, _Value2>&);
	Iterator_TripleMap(const Iterator_TripleMap<_Key, _Value1, _Value2>&);
	~Iterator_TripleMap();

public:
	bool Equals(const Iterator&) const;
	bool Equals(const Element&) const;
	bool Equals(const SelfMap&, const _Key&) const;
	inline bool Empty() const;
	inline bool End() const;
	std::pair<_Value1, _Value2> GetValues() const;

	bool Erase();
	Iterator& Next();
	Iterator& Back();
	inline SelfMap& GetParent() const;
	inline bool Clear();
	bool ClearMap();
	bool ClearValue();
	bool ClearAllValues();

public:
	TripleMap<_Key, _Value1, _Value2>& operator =(const Iterator&);
	inline bool operator ==(const Iterator&) const;
	inline bool operator !=(const Iterator&) const;
	inline bool operator ==(const Element&) const;
	inline bool operator !=(const Element&) const;
	inline bool operator ==(std::nullptr_t) const;
	inline bool operator !=(std::nullptr_t) const;
	inline operator bool() const;
	inline bool operator !() const;

	Iterator& operator +(int);
	Iterator& operator -(int);
	int operator -(const Iterator&) const;
	inline Iterator& operator +=(int);
	inline Iterator& operator -=(int);
	inline Iterator& operator ++();
	inline Iterator operator ++(int);
	inline Iterator& operator --();
	inline Iterator operator --(int);

	inline const Element* operator->() const;
	inline Element operator *();
	inline const Element operator *() const;

public:
	static const Iterator&iempty();
	static Iterator GetEmpty();

public:
	friend Iterator&SelfMap::Before(const Iterator&, int num /* = 1 */);
	friend Iterator&SelfMap::After(const Iterator&, int num /* = 1 */);

private:
	uint32 num;
	SelfMap* parent;

private:
	Iterator_TripleMap();
};


/****************************** Source Code : Triad ******************************************/


template <class _First, class _Second, class _Third>
Triad<_First, _Second, _Third>::Triad()
{
}

template <class _First, class _Second, class _Third>
Triad<_First, _Second, _Third>::Triad(const _First& first, const _Second& second, const _Third& third)
	: first(first), second(second), third(third)
{
}

template <class _First, class _Second, class _Third>
Triad<_First, _Second, _Third>::Triad(const Triad&value)
	: first(value.first), second(value.second), third(value.third)
{
}

template <class _First, class _Second, class _Third>
Triad<_First, _Second, _Third>::Triad(const Iterator&value)
	: first(value->first), second(value->second), third(value->third)
{
}

template <class _First, class _Second, class _Third>
Triad<_First, _Second, _Third>& Triad<_First, _Second, _Third>::operator=(const SelfType&value)
{
	first = value.first;
	second = value.second;
	third = value.third;
	return *this;
}

template <class _First, class _Second, class _Third>
Triad<_First, _Second, _Third>& Triad<_First, _Second, _Third>::operator=(const Iterator&value)
{
	first = value->first;
	second = value->second;
	third = value->third;
	return *this;
}

template <class _First, class _Second, class _Third>
Triad<_First, _Second, _Third>::~Triad()
{
}

template <class _First, class _Second, class _Third>
bool Triad<_First, _Second, _Third>::Equals(const SelfType& value) const
{
	if(first == value.first && second == value.second && third == value.third)
		return true;
	return false;
}

template <class _First, class _Second, class _Third>
Triad<_Second, _First, _Third> Triad<_First, _Second, _Third>::Swap12() const
{
	return Triad(second, first, third);
}

template <class _First, class _Second, class _Third>
Triad<_Third, _Second, _First> Triad<_First, _Second, _Third>::Swap13() const
{
	return Triad(third, second, first);
}

template <class _First, class _Second, class _Third>
Triad<_First, _Third, _Second> Triad<_First, _Second, _Third>::Swap23() const
{
	return Triad(first, third, second);
}

template <class _First, class _Second, class _Third>
std::pair<_First, _Second> Triad<_First, _Second, _Third>::GetValue12() const
{
	return std::pair<_First, _Second>(first, second);
}

template <class _First, class _Second, class _Third>
std::pair<_First, _Third> Triad<_First, _Second, _Third>::GetValue13() const
{
	return std::pair<_First, _Third>(first, third);
}

template <class _First, class _Second, class _Third>
std::pair<_Second, _Third> Triad<_First, _Second, _Third>::GetValue23() const
{
	return std::pair<_Second, _Third>(second, third);
}

template <class _First, class _Second, class _Third>
bool Triad<_First, _Second, _Third>::operator==(const SelfType&value) const
{
	return Equals(value);
}

template <class _First, class _Second, class _Third>
bool Triad<_First, _Second, _Third>::operator!=(const SelfType&value) const
{
	return !operator==(value);
}

template <class _First, class _Second, class _Third>
bool Triad<_First, _Second, _Third>::operator==(const Iterator&i) const
{
	return operator==(*i);
}

template <class _First, class _Second, class _Third>
bool Triad<_First, _Second, _Third>::operator!=(const Iterator&i) const
{
	return operator!=(i);
}


/****************************** Source Code : TripleMap ******************************************/


template <class _Key, class _Value1, class _Value2>
TripleMap<_Key,_Value1,_Value2>::TripleMap(Triad<_Key, _Value1, _Value2>* dataArray/* = nullptr*/, uint32 num/* = 0*/)
{
	for(uint32 i = 0; i < num; i++)
	{
		datas.push_back(dataArray[i]);
	}
}

template <class _Key, class _Value1, class _Value2>
TripleMap<_Key, _Value1, _Value2>::~TripleMap()
{
	Clear();
}

template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::Equals(const TripleMap<_Key, _Value1, _Value2>& value) const
{
	if(Empty() && value)
		return true;
	if(Empty() && !value)
		return false;
	if(!Empty() && value)
		return false;
	auto size = value.datas.size();
	if(datas.size() != size)
		return false;
	for(uint32 i = 0; i < size; i++)
	{
		if(datas.at(i) != value.datas.at(i))
			return false;
	}
	return true;
}

template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::Empty() const
{
	return datas.empty();
}

template <class _Key, class _Value1, class _Value2>
std::pair<_Value1, _Value2> TripleMap<_Key, _Value1, _Value2>::GetValues(const _Key& key) const
{
	auto ret = Find(key);
	return ret->GetValue23();
}

template <class _Key, class _Value1, class _Value2>
uint32 TripleMap<_Key, _Value1, _Value2>::Size() const
{
	return uint32(datas.size());
}


template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::Insert(const _Key&key, const _Value1&value1, const _Value2&value2)
{
	for(auto i = datas.begin(); i != datas.end(); i++)
	{
		if(i->first == key)
		{
			i->second = value1;
			i->third = value2;
			return true;
		}
	}
	datas.push_back(Element(key, value1, value2));
	return true;
}

template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::Insert(const Iterator&i)
{
	for(auto p = datas.begin(); p != datas.end(); p++)
	{
		if(p->first == i->first)
		{
			p->value1 = i->value1;
			p->value2 = i->value2;
			return true;
		}
	}
	datas.push_back(*i);
	return true;
}

template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::Insert(const Element&e)
{
	if(e.Empty())
		return false;
	return Insert(e.first, e.second, e.third);
}

template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::Insert(const _Key&key, const std::pair<_Value1, _Value2>&value)
{
	return Insert(key, value.first, value.second);
}

template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::Erase(Iterator&i)
{
	return i.Erase();
}

template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::Erase(const _Key&key)
{
	auto value = Find(key);
	if(value)
	{
		return value.Erase();
	}
	return false;
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2> TripleMap<_Key, _Value1, _Value2>::Find(const _Key&key)
{
	auto ret = Begin();
	if(ret == End())
		return End();
	for(auto p = datas.begin(); p != datas.end(); p++,ret++)
	{
		if(p->first == key)
			return ret;
	}
	return End();
}

template <class _Key, class _Value1, class _Value2>
const Iterator_TripleMap<_Key, _Value1, _Value2> TripleMap<_Key, _Value1, _Value2>::Find(const _Key&key) const
{
	auto ret = Begin();
	if(ret == End())
		return End();
	for(auto p = datas.begin(); p != datas.end(); p++, ret++)
	{
		if(p->first == key)
			return ret;
	}
	return End();
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2> TripleMap<_Key, _Value1, _Value2>::Begin()
{
	if(Empty())
		return End();
	auto ret = Iterator_TripleMap<_Key, _Value1, _Value2>(*this);
	return ret;
}

template <class _Key, class _Value1, class _Value2>
const Iterator_TripleMap<_Key, _Value1, _Value2> TripleMap<_Key, _Value1, _Value2>::Begin() const
{
	if(Empty())
		return End();
	return const_cast<TripleMap<_Key, _Value1, _Value2>*>(this)->Begin();
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>& TripleMap<_Key, _Value1, _Value2>::Before(const Iterator&i, int num /*= 1*/)
{
	if(i.num <= num)
		return Begin();
	i.num -= num;
	i.pointer = &(datas[i.num]);
	return i;
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>& TripleMap<_Key, _Value1, _Value2>::After(const Iterator&i, int num /*= 1*/)
{
	if(i.num + num >= datas.size() || i==nullptr)
		return End();
	i.num += num;
	i.pointer = &(datas[i.num]);
	return i;
}

template <class _Key, class _Value1, class _Value2>
const Iterator_TripleMap<_Key, _Value1, _Value2>&TripleMap<_Key, _Value1, _Value2>::End()
{
	return Iterator::iempty();
}

template <class _Key, class _Value1, class _Value2>
void TripleMap<_Key, _Value1, _Value2>::Sort(SortFunc sortFunc)
{
	uint32 times = 1;
	auto size = datas.size()-1;
	while(size>0&&times != 0)
	{
		times = 0;
		auto ref1 = Begin();
		auto ref2 = ref1 + 1;
		while(ref2 != End())
		{
			if(sortFunc(ref1, ref2))
			{
				times++;
				ref1.SwapBack();
			}
			ref1++;
			ref2++;
		}
	}
}

template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::Clear()
{
	datas.clear();
	return true;
}

template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::ClearValues()
{
	for(auto i = datas.begin(); i != datas.end(); i++)
	{
		auto ret = _Key(i->first);
		i->Clear(&ret);
	}
	return true;
}

template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::operator==(const TripleMap<_Key, _Value1, _Value2>&value) const
{
	return Equals(value);
}

template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::operator!=(const TripleMap<_Key, _Value1, _Value2>&value) const
{
	return !operator ==(value);
}

template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::operator==(std::nullptr_t) const
{
	return Empty();
}

template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::operator!=(std::nullptr_t) const
{
	return !operator ==(nullptr);
}

template <class _Key, class _Value1, class _Value2>
TripleMap<_Key, _Value1, _Value2>::operator bool() const
{
	return operator !=(nullptr);
}

template <class _Key, class _Value1, class _Value2>
bool TripleMap<_Key, _Value1, _Value2>::operator !() const
{
	return !operator bool();
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2> TripleMap<_Key, _Value1, _Value2>::operator[](const _Key&key)
{
	return Find(key);
}

template <class _Key, class _Value1, class _Value2>
const Iterator_TripleMap<_Key, _Value1, _Value2> TripleMap<_Key, _Value1, _Value2>::operator[](const _Key&key) const
{
	return Find(key);
}

template <class _Key, class _Value1, class _Value2>
const Iterator_TripleMap<_Key, _Value1, _Value2> TripleMap<_Key, _Value1, _Value2>::operator[](std::nullptr_t)
{
	return Iterator::iempty();
}


/****************************** Source Code : Iterator_TripleMap ******************************************/


template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>::Iterator_TripleMap(TripleMap<_Key, _Value1, _Value2>&map)
	:num(0),parent(&map)
{
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>::Iterator_TripleMap()
	:num(0),parent(nullptr)
{
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>::Iterator_TripleMap(const Iterator_TripleMap<_Key, _Value1, _Value2>&value)
	:num(value.num),parent(value.parent)
{
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>::~Iterator_TripleMap()
{
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::Equals(const Iterator&value) const
{
	return (num == value.num&&parent == value.parent) || (value.parent == nullptr&&parent != nullptr&& num >= parent->datas.size());
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::Equals(const Element&value) const
{
	return((parent == nullptr&&value.Empty()) || (**this == value));
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::Equals(const SelfMap&map, const _Key&key) const
{
	return Equals(map.Find(key));
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::Empty() const
{
	return parent == nullptr;
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::End() const
{
	return parent == nullptr || num >= parent->datas.size();
}

template <class _Key, class _Value1, class _Value2>
std::pair<_Value1, _Value2> Iterator_TripleMap<_Key, _Value1, _Value2>::GetValues() const
{
	return (*this)->GetValue23();
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::Erase()
{
	if(End())
		return false;
	parent->datas.erase(parent->datas.begin() + num);
	return true;
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>& Iterator_TripleMap<_Key, _Value1, _Value2>::Next()
{
	if(!End())
		num++;
	return *this;
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>& Iterator_TripleMap<_Key, _Value1, _Value2>::Back()
{
	if(num > 0)
		num--;
	return *this;
}

template <class _Key, class _Value1, class _Value2>
TripleMap<_Key, _Value1, _Value2>& Iterator_TripleMap<_Key, _Value1, _Value2>::GetParent() const
{
	return *parent;
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::Clear()
{
	num = 0;
	parent = nullptr;
	return true;
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::ClearMap()
{
	if(!Empty())
		return parent->Clear();
	return true;
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::ClearValue()
{
	if(!End())
		return parent->at(num).Clear();
	return true;
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::ClearAllValues()
{
	if(!Empty())
		return parent->ClearValues();
	return true;
}

template <class _Key, class _Value1, class _Value2>
TripleMap<_Key, _Value1, _Value2>& Iterator_TripleMap<_Key, _Value1, _Value2>::operator=(const Iterator&value)
{
	num = value.num;
	parent = value.parent;
	return *this;
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::operator==(const Iterator&value) const
{
	return Equals(value);
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::operator!=(const Iterator&value) const
{
	return !operator ==(value);
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::operator==(const Element&value) const
{
	return Equals(value);
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::operator!=(const Element&value) const
{
	return !operator ==(value);
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::operator==(std::nullptr_t) const
{
	return Empty();
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::operator!=(std::nullptr_t) const
{
	return !operator==(nullptr);
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>::operator bool() const
{
	return operator !=(nullptr);
}

template <class _Key, class _Value1, class _Value2>
bool Iterator_TripleMap<_Key, _Value1, _Value2>::operator!() const
{
	return !operator bool();
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>& Iterator_TripleMap<_Key, _Value1, _Value2>::operator+(int len)
{
	for(auto i = len; i > 0; i++, Next());
	return *this;
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>& Iterator_TripleMap<_Key, _Value1, _Value2>::operator-(int len)
{
	for(auto i = len; i > 0; i++, Back());
	return *this;
}

template <class _Key, class _Value1, class _Value2>
int Iterator_TripleMap<_Key, _Value1, _Value2>::operator-(const Iterator&i) const
{
	if(parent != i.parent || parent == nullptr)
		return 0x7fffffff;
	return num - i.num;
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>& Iterator_TripleMap<_Key, _Value1, _Value2>::operator+=(int len)
{
	return operator+(len);
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>& Iterator_TripleMap<_Key, _Value1, _Value2>::operator-=(int len)
{
	return operator-(len);
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>& Iterator_TripleMap<_Key, _Value1, _Value2>::operator++()
{
	if(!End())
		num++;
	return *this;
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2> Iterator_TripleMap<_Key, _Value1, _Value2>::operator++(int)
{
	Iterator_TripleMap<_Key, _Value1, _Value2> ret;
	ret.parent = this->parent;
	ret.num = this->num;
	if(!End())
		num++;
	return ret;
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2>& Iterator_TripleMap<_Key, _Value1, _Value2>::operator--()
{
	if(!End())
		num--;
	return *this;
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2> Iterator_TripleMap<_Key, _Value1, _Value2>::operator--(int)
{
	Iterator_TripleMap<_Key, _Value1, _Value2> ret;
	ret.parent = this->parent;
	ret.num = this->num;
	if(!End())
		num--;
	return ret;
}

template <class _Key, class _Value1, class _Value2>
const Triad<_Key, _Value1, _Value2>* Iterator_TripleMap<_Key, _Value1, _Value2>::operator->() const
{
	if(End())
		return nullptr;
	return &parent->datas.at(num);
}

template <class _Key, class _Value1, class _Value2>
Triad<_Key, _Value1, _Value2> Iterator_TripleMap<_Key, _Value1, _Value2>::operator*()
{
	if(End())
		return Element();
	return parent->datas.at(num);
}

template <class _Key, class _Value1, class _Value2>
const Triad<_Key, _Value1, _Value2> Iterator_TripleMap<_Key, _Value1, _Value2>::operator*() const
{
	if(End())
		return Element();
	return parent->datas.at(num);
}

template <class _Key, class _Value1, class _Value2>
const Iterator_TripleMap<_Key, _Value1, _Value2>&Iterator_TripleMap<_Key, _Value1, _Value2>::iempty() {
	// TODO : This memory cannot be released, must change that code
	static Iterator_TripleMap<_Key, _Value1, _Value2> ret;
	return ret;
}

template <class _Key, class _Value1, class _Value2>
Iterator_TripleMap<_Key, _Value1, _Value2> Iterator_TripleMap<_Key, _Value1, _Value2>::GetEmpty()
{
	return Iterator_TripleMap<_Key, _Value1, _Value2>();
}

 } // namespace ArmyAnt
#endif