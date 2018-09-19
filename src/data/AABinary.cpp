/*
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
 * This file is the internal source file of this project, is not contained by the closed source release part of this software
 * 本文件为内部源码文件, 不会包含在闭源发布的本软件中
 */

#include "../base/base.hpp"
#include "../../inc/AABinary.h"
#include "../../inc/AAClassPrivateHandle.hpp"
#include "../../inc/AATripleMap.hpp"
#include "../../inc/AAString.h"
#include <iostream>
#include <cstring>


#define AA_HANDLE_MANAGER ClassPrivateHandleManager<BinaryParser, BinaryParser_Private>::getInstance()

namespace ArmyAnt {

class BinaryParser_Private
{
public:
	BinaryParser_Private();
	~BinaryParser_Private();
	enum class DataType : uint8
	{
		Bool,
		Int,
		UInt,
		Double,
		String,
		Binary
	};
	typedef std::pair<DataType, uint32> DataAttr;

public:
	bool InsertData(String tag, DataType tp, uint32 len, const void*buffer);
	bool RemoveData(String tag);
	bool ClearData();
	uint32 EncodingToBinary(uint8* buffer)const;
	uint32 DecodingFromBinary(const uint8* buffer);

public:
	TripleMap<String, DataAttr, uint8*> data;

	AA_FORBID_ASSGN_OPR(BinaryParser_Private);
	AA_FORBID_COPY_CTOR(BinaryParser_Private);
};

BinaryParser_Private::BinaryParser_Private()
{
}

BinaryParser_Private::~BinaryParser_Private()
{
	AAAssert(ClearData(), );
}

bool BinaryParser_Private::InsertData(String tag, DataType tp, uint32 len, const void * buffer)
{
	uint8*buf = nullptr;
	buf = new uint8[len];
	memcpy(buf, buffer, len);
	data.Insert(tag, DataAttr(tp, len), buf);
	return true;
}

bool BinaryParser_Private::RemoveData(String tag)
{
	auto ret = data.Find(tag);
	if(ret == nullptr)
		return false;
	delete (ret->third);
	return data.Erase(ret);
}

bool BinaryParser_Private::ClearData()
{
	for(auto i = data.Begin(); i < data.End(); i++)
	{
		if(!i.Erase())
			return false;
	}
	return true;
}

uint32 BinaryParser_Private::EncodingToBinary(uint8 * buffer) const
{
	uint32 ret = 0;
	for(auto i = data.Begin(); i != data.End(); i++)
	{
		strcpy(reinterpret_cast<char*>(buffer) + ret, i->first.c_str());
		ret += i->first.size();
		buffer[ret++] = uint8(i->second.first);
		*reinterpret_cast<uint32*>(buffer + ret) = i->second.second;
		ret += 8;
		memcpy(buffer + ret, i->third, i->second.second);
		ret += i->second.second;
	}
	buffer[ret++] = 0;
	return ret;
}

uint32 BinaryParser_Private::DecodingFromBinary(const uint8 * buffer)
{
	uint32 ret = 0;
	String tag = "";
	DataType tp;
	uint32 len = 0;
	while(buffer[ret] != 0)
	{
		tag = "";
		while(buffer[ret] != 0)
			tag += char(buffer[ret++]);
		tp = DataType(buffer[ret++]);
		len = *reinterpret_cast<uint32*>(buffer[++ret]);
		ret += 8;
		InsertData(tag, tp, len, buffer + ret);
		ret += len;
	}
	return ret + 1;
}



BinaryParser::BinaryParser()
{
    AA_HANDLE_MANAGER.GetHandle(this);
}

BinaryParser::~BinaryParser()
{
    delete AA_HANDLE_MANAGER.ReleaseHandle(this);
}

uint64 BinaryParser::GetBytes(void * buffer) const
{
	auto hd = AA_HANDLE_MANAGER[this];
	return hd->EncodingToBinary(static_cast<uint8*>(buffer));
}

bool BinaryParser::SetBytes(const void * buffer, uint32 len)
{
	AAAssert(len > 0, false);
	auto hd = AA_HANDLE_MANAGER[this];
	uint8*ret = new uint8[len];
	memcpy(ret, buffer, len);
	return hd->DecodingFromBinary(ret) > 0;
}

bool BinaryParser::SetInfo(const char * tag, bool value)
{
	return AA_HANDLE_MANAGER[this]->InsertData(tag, BinaryParser_Private::DataType::Bool, sizeof(bool), &value);
}

bool BinaryParser::SetInfo(const char * tag, uint64 value, uint8 bits)
{
	return AA_HANDLE_MANAGER[this]->InsertData(tag, bits%8==0?BinaryParser_Private::DataType::UInt:BinaryParser_Private::DataType::Int, sizeof(uint64), &value);
}

bool BinaryParser::SetInfo(const char * tag, char value)
{
	uint64 ret = value;
	return AA_HANDLE_MANAGER[this]->InsertData(tag, BinaryParser_Private::DataType::Int, sizeof(uint64), &ret);
}

bool BinaryParser::SetInfo(const char * tag, double value)
{
	return AA_HANDLE_MANAGER[this]->InsertData(tag, BinaryParser_Private::DataType::Double, sizeof(double), &value);
}

bool BinaryParser::SetInfo(const char * tag, const char * value)
{
	return AA_HANDLE_MANAGER[this]->InsertData(tag, BinaryParser_Private::DataType::String, strlen(value), value);
}

bool BinaryParser::SetInfo(const char * tag, void * values, uint32 len)
{
	return AA_HANDLE_MANAGER[this]->InsertData(tag, BinaryParser_Private::DataType::Binary, len, values);
}

bool BinaryParser::GetBoolInfo(const char * tag)const
{
	auto hd = AA_HANDLE_MANAGER[this]->data.Find(tag);
	AAAssert(hd != nullptr&&hd->second.first == BinaryParser_Private::DataType::Bool, false);
	return *reinterpret_cast<bool*>(hd->third);
}

char BinaryParser::GetInt8Info(const char * tag)const
{
	auto hd = AA_HANDLE_MANAGER[this]->data.Find(tag);
	AAAssert(hd != nullptr&&hd->second.first == BinaryParser_Private::DataType::Int, char(0));
	return char(*reinterpret_cast<uint64*>(hd->third));
}

uint8 BinaryParser::GetUInt8Info(const char * tag)const
{
	auto hd = AA_HANDLE_MANAGER[this]->data.Find(tag);
	AAAssert(hd != nullptr&&hd->second.first == BinaryParser_Private::DataType::UInt, uint8(0));
	return uint8(*reinterpret_cast<uint64*>(hd->third));
}

int16 BinaryParser::GetInt16Info(const char * tag)const
{
	auto hd = AA_HANDLE_MANAGER[this]->data.Find(tag);
	AAAssert(hd != nullptr&&hd->second.first == BinaryParser_Private::DataType::Int, int16(0));
	return int16(*reinterpret_cast<uint64*>(hd->third));
}

uint16 BinaryParser::GetUInt16Info(const char * tag)const
{
	auto hd = AA_HANDLE_MANAGER[this]->data.Find(tag);
	AAAssert(hd != nullptr&&hd->second.first == BinaryParser_Private::DataType::UInt, uint16(0));
	return uint16(*reinterpret_cast<uint64*>(hd->third));
}

int32 BinaryParser::GetInt32Info(const char * tag)const
{
	auto hd = AA_HANDLE_MANAGER[this]->data.Find(tag);
	AAAssert(hd != nullptr&&hd->second.first == BinaryParser_Private::DataType::Int, 0);
	return int32(*reinterpret_cast<uint64*>(hd->third));
}

uint32 BinaryParser::GetUInt32Info(const char * tag)const
{
	auto hd = AA_HANDLE_MANAGER[this]->data.Find(tag);
	AAAssert(hd != nullptr&&hd->second.first == BinaryParser_Private::DataType::UInt, 0U);
	return uint32(*reinterpret_cast<uint64*>(hd->third));
}

int64 BinaryParser::GetInt64Info(const char * tag)const
{
	auto hd = AA_HANDLE_MANAGER[this]->data.Find(tag);
	AAAssert(hd != nullptr&&hd->second.first == BinaryParser_Private::DataType::Int, int64(0));
	return int64(*reinterpret_cast<uint64*>(hd->third));
}

uint64 BinaryParser::GetUInt64Info(const char * tag)const
{
	auto hd = AA_HANDLE_MANAGER[this]->data.Find(tag);
	AAAssert(hd != nullptr&&hd->second.first == BinaryParser_Private::DataType::UInt, uint64(0));
	return uint64(*reinterpret_cast<uint64*>(hd->third));
}

double BinaryParser::GetFloatInfo(const char * tag)const
{
	auto hd = AA_HANDLE_MANAGER[this]->data.Find(tag);
	AAAssert(hd != nullptr&&hd->second.first == BinaryParser_Private::DataType::Double, .0);
	return *reinterpret_cast<double*>(hd->third);
}

const char * BinaryParser::GetStrInfo(const char * tag, char * buffer)const
{
	auto hd = AA_HANDLE_MANAGER[this]->data.Find(tag);
	AAAssert(hd != nullptr&&hd->second.first == BinaryParser_Private::DataType::String, nullptr);
	memcpy(buffer, hd->third, hd->second.second);
	return buffer;
}

const void * BinaryParser::GetBinaryInfo(const char * tag, void * buffer, mac_uint len)const
{
	auto hd = AA_HANDLE_MANAGER[this]->data.Find(tag);
	AAAssert(hd != nullptr&&hd->second.first == BinaryParser_Private::DataType::Binary, nullptr);
	len = hd->second.second;
	memcpy(buffer, hd->third, len);
	return buffer;
}

const char* BinaryParser::GetDataType(const char * tag)const
{
	auto hd = AA_HANDLE_MANAGER[this]->data.Find(tag);
	if(hd == nullptr)
		return typeid(nullptr).name();
	switch(hd->second.first)
	{
		case BinaryParser_Private::DataType::Bool:
			return typeid(bool).name();
		case BinaryParser_Private::DataType::Int:
			switch(hd->second.second)
			{
				case 1:
					return typeid(int8).name();
				case 2:
					return typeid(int16).name();
				case 3:
					return typeid(int32).name();
				case 4:
					return typeid(int64).name();
				default:
					return nullptr;
			}
		case BinaryParser_Private::DataType::UInt:
			switch(hd->second.second)
			{
				case 1:
					return typeid(uint8).name();
				case 2:
					return typeid(uint16).name();
				case 3:
					return typeid(uint32).name();
				case 4:
					return typeid(uint64).name();
				default:
					return nullptr;
			}
		case BinaryParser_Private::DataType::Double:
			return typeid(double).name();
		case BinaryParser_Private::DataType::String:
			return typeid(char*).name();
		case BinaryParser_Private::DataType::Binary:
			return typeid(void).name();
		default:
			return typeid(nullptr).name();
	}
}

bool BinaryParser::RemoveInfo(const char * tag)
{
	return AA_HANDLE_MANAGER[this]->RemoveData(tag);
}

bool BinaryParser::ClearInfo()
{
	return AA_HANDLE_MANAGER[this]->ClearData();
}

}

#undef AA_HANDLE_MANAGER