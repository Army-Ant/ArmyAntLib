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
#include "../../inc/AAAes.h"
#include "../../inc/AAClassPrivateHandle.hpp"
#include <boost/random.hpp>
#include <memory.h>


#define AA_BYTE_ENCODER_HANDLE_MANAGER ClassPrivateHandleManager<ByteEncoder, ByteEncoder_Private_Ref>::getInstance()
#define AA_ROUND_SETTING_HANDLE_MANAGER ClassPrivateHandleManager<RoundSetting, RoundSetting_Private_Ref>::getInstance()
#define AA_PARSER_HANDLE_MANAGER ClassPrivateHandleManager<Parser, Parser_Private_Ref>::getInstance()

namespace ArmyAnt {

namespace AES {

class ByteEncoder_Private_Ref;
class ByteEncoder_Private;
class RoundSetting_Private_Ref;
class RoundSetting_Private;
class Parser_Private_Ref;
class Parser_Private;

/***************** Code : ByteEncoder *******************************************************************/

class ByteEncoder_Private
{
public:
	ByteEncoder_Private();
	~ByteEncoder_Private();

public:
	//生成一个随机的S盒子
	void MakeRandomSBox();
	//转化为反盒子
	bool TurnToBack(bool withCheck = false);
	//输出一个带有反盒子的新内部类，该类不被管理器所管理
	ByteEncoder_Private GetBackDatas();
	//验证当前的盒子是否为一一映射
	bool CheckObayRule();

	uint8 data[256] = {0};
	uint16 refCount = 1;

public:
	static const int16 c_SBoxLength = 256;
};
class ByteEncoder_Private_Ref
{
public:
    ByteEncoder_Private_Ref(ByteEncoder_Private* reffer = nullptr)
    {
        if (reffer == nullptr)
            this->reffer = new ByteEncoder_Private();
        else
        {
            this->reffer = reffer;
            ++(reffer->refCount);
        }
    }
    ~ByteEncoder_Private_Ref()
    {
        --(reffer->refCount);
        if (reffer->refCount <= 0)
            delete reffer;
    }
    ByteEncoder_Private&operator ->()
    {
        return *reffer;
    }
    ByteEncoder_Private&operator*()
    {
        return *reffer;
    }
    ByteEncoder_Private* reffer = nullptr;
};

ByteEncoder_Private::ByteEncoder_Private()
{
}

ByteEncoder_Private::~ByteEncoder_Private()
{
}

void ByteEncoder_Private::MakeRandomSBox()
{
	//清空数据
	memset(data, 0, c_SBoxLength);
	//重置随机种子
	boost::mt19937 gen;
	boost::random::uniform_smallint<uint32> r(0, c_SBoxLength - 1);
	boost::variate_generator<boost::mt19937&, boost::uniform_smallint<uint32>>die(gen, r);

	//摇出第一个数
	uint8 start = uint8(die());
	//摇出剩余的数
	for(int16 i = 1; i < c_SBoxLength; i++)
	{
		uint8 c = uint8(die());
		while(c == start || data[c] > 0)
		{
			if(++c > c_SBoxLength - 1)
				c = 0;
		}
		data[c] = uint8(i);
	}
}

bool ByteEncoder_Private::TurnToBack(bool withCheck/* = false*/)
{
	//存放反转后数据的临时数组
	uint8 tmp[c_SBoxLength] = {0};
	if(withCheck)
	{
		//索引为0的数的值，将变为转化后值为0的索引
		uint8 zeropos = this->data[0];
		for(int16 i = 0; i < c_SBoxLength; i++)
		{
			//如果值为zeropos，则表明另一个索引不为0的数(i)占用该处，这不符合规则
			if(i > 0 && this->data[i] == zeropos)
				return false;
			//如果值不为0，则证明该处已被占用，这不符合规则
			if(tmp[this->data[i]] > 0)
				return false;
			tmp[this->data[i]] = uint8(i);
		}
	}
	else for(int16 i = 0; i < c_SBoxLength; i++)
	{
		//不检查，单纯转换数据，如果原S盒不符合规则，可能发生数据覆盖等问题
		tmp[this->data[i]] = uint8(i);
	}
	//更新数据
	memcpy(this->data, tmp, c_SBoxLength);
	return true;
}


ByteEncoder_Private ByteEncoder_Private::GetBackDatas()
{
	ByteEncoder_Private ret;
	memcpy(&ret, this, sizeof(ByteEncoder_Private));
	ret.TurnToBack();
	return ret;
}

bool ByteEncoder_Private::CheckObayRule()
{
	//通过生成反转盒是否成功，来判断元数据是否符合规则
	auto ret = new ByteEncoder_Private();
	memcpy(ret, this, sizeof(ByteEncoder_Private));
	auto res = ret->TurnToBack(true);
	delete ret;
	return res;
}

ByteEncoder::ByteEncoder()
{
    AA_BYTE_ENCODER_HANDLE_MANAGER.GetHandle(this);
}


ByteEncoder::ByteEncoder(const ByteEncoder&value)
{
    AA_BYTE_ENCODER_HANDLE_MANAGER.GetHandle(this, new ByteEncoder_Private_Ref(AA_BYTE_ENCODER_HANDLE_MANAGER[&value]->reffer));
}


ByteEncoder::~ByteEncoder()
{
    delete AA_BYTE_ENCODER_HANDLE_MANAGER. ReleaseHandle(this);
}


bool ByteEncoder::InputData(const uint8 elems[ByteEncoder_Private::c_SBoxLength], bool needCheck/* = false*/)
{
	AAAssert(elems != nullptr,false);
	auto hd = AA_BYTE_ENCODER_HANDLE_MANAGER[this]->reffer;
	if(needCheck)
	{
		//将元数据拷贝到临时区域，失败时将还原
		uint8 tmp[ByteEncoder_Private::c_SBoxLength];
		memcpy(tmp, hd->data, ByteEncoder_Private::c_SBoxLength);
		memcpy(hd->data, elems, ByteEncoder_Private::c_SBoxLength);
		//验证是否符合规则
		if(hd->CheckObayRule())
			return true;
		memcpy(hd->data, tmp, ByteEncoder_Private::c_SBoxLength);
		return false;
	}
	else
	{
		//不验证，仅输入数据
		memcpy(hd->data, elems, ByteEncoder_Private::c_SBoxLength);
	}
	return true;
}


bool ByteEncoder::InputBackData(const uint8 elems[ByteEncoder_Private::c_SBoxLength], bool needCheck/* = false*/)
{
	//输入数据，然后求反
	if(!InputData(elems, needCheck))
		return false;
	return AA_BYTE_ENCODER_HANDLE_MANAGER[this]->reffer->TurnToBack();
}


bool ByteEncoder::CopiedFromAnother(const ByteEncoder&another, bool needCheck /*= false*/)
{
	//找不到元数据
	if(AA_BYTE_ENCODER_HANDLE_MANAGER[&another] == nullptr)
		return false;
	//要求检查，而元数据不符合规则
	if(needCheck&&!AA_BYTE_ENCODER_HANDLE_MANAGER[&another]->reffer->CheckObayRule())
		return false;
	//解除原来的本数据类，引用计数器-1
    delete AA_BYTE_ENCODER_HANDLE_MANAGER. ReleaseHandle(this);
	//绑定目标数据类的句柄，引用计数器+1
    AA_BYTE_ENCODER_HANDLE_MANAGER.GetHandle(this, new ByteEncoder_Private_Ref(AA_BYTE_ENCODER_HANDLE_MANAGER[&another]->reffer));
	return true;
}


bool ByteEncoder::GetData(uint8 elems[ByteEncoder_Private::c_SBoxLength]) const
{
	AAAssert(elems != nullptr,false);
	memcpy(elems, AA_BYTE_ENCODER_HANDLE_MANAGER[this]->reffer->data, ByteEncoder_Private::c_SBoxLength);
	return true;
}


bool ByteEncoder::GetBackData(uint8 elems[ByteEncoder_Private::c_SBoxLength]) const
{
	ByteEncoder_Private tmp;
	memcpy(&tmp, AA_BYTE_ENCODER_HANDLE_MANAGER[this]->reffer, sizeof(ByteEncoder_Private));
	tmp.TurnToBack();
	memcpy(elems, tmp.data, ByteEncoder_Private::c_SBoxLength);
	return true;
}


ByteEncoder ByteEncoder::GetBack() const
{
	ByteEncoder ret;
	memcpy(AA_BYTE_ENCODER_HANDLE_MANAGER[&ret]->reffer->data, AA_BYTE_ENCODER_HANDLE_MANAGER[this]->reffer->data, ByteEncoder_Private::c_SBoxLength);
	AA_BYTE_ENCODER_HANDLE_MANAGER[&ret]->reffer->TurnToBack();
	return ret;
}

ByteEncoder& ByteEncoder::operator= (const ByteEncoder&value)
{
	AAAssert((AA_BYTE_ENCODER_HANDLE_MANAGER[&value] != nullptr), *this);
	AAAssert(CopiedFromAnother(value), *this);
	return *this;
}

uint8& ByteEncoder::operator[](uint8 src)
{
	return AA_BYTE_ENCODER_HANDLE_MANAGER[this]->reffer->data[src];
}

const uint8 ByteEncoder::operator[](uint8 src) const
{
	return AA_BYTE_ENCODER_HANDLE_MANAGER[this]->reffer->data[src];
}

ByteEncoder ByteEncoder::operator*() const
{
	return GetBack();
}

const ByteEncoder ByteEncoder::GetRandomEncoder()
{
	ByteEncoder ret;
	AA_BYTE_ENCODER_HANDLE_MANAGER[&ret]->reffer->MakeRandomSBox();
	return ret;
}

/***************** Code : RoundSetting ******************************************************************/


class RoundSetting_Private
{
public:
	uint8 pwd[16] = {0};
    ByteEncoder_Private_Ref* encoder = nullptr;
	uint8 rectWidth = 4;
	uint16 refCount = 1;

public:
	~RoundSetting_Private();
	void ResetEncoder();
	inline bool CheckCanDo();

	inline bool ByteEncode(void* dest, const char*src, uint64 length);
	inline bool ByteDecode(void* dest, const char*src, uint64 length);
	static bool ByteEncode(ByteEncoder_Private_Ref* encoder, void* dest, const char*src, uint64 length);
	static bool ByteDecode(ByteEncoder_Private_Ref* encoder, void* dest, const char*src, uint64 length);

	inline bool LineMove(void*dest, const char*src, uint64 length);
	inline bool LineMoveBack(void*dest, const char*src, uint64 length);
	static bool LineMove(uint8 rectWidth, void*dest, const char*src, uint64 length, bool isBack = false);
	static bool RowMix(void*dest, const char*src, uint64 length, bool isBack = false);

	inline bool Lock(void*dest, const char*src, uint64 length);
	static bool Lock(void*dest, const char*src, uint64 length, uint8 pwd[16]);

public:
	inline static uint8 BinMul(uint8 x, uint8 y);
};
class RoundSetting_Private_Ref
{
public:
    RoundSetting_Private_Ref(RoundSetting_Private* reffer = nullptr)
    {
        if (reffer == nullptr)
            this->reffer = new RoundSetting_Private();
        else
        {
            this->reffer = reffer;
            ++(reffer->refCount);
        }
    }
    ~RoundSetting_Private_Ref()
    {
        --(reffer->refCount);
        if (reffer->refCount <= 0)
            delete reffer;
    }
    RoundSetting_Private&operator ->()
    {
        return *reffer;
    }
    RoundSetting_Private&operator*()
    {
        return *reffer;
    }
    RoundSetting_Private*reffer = nullptr;
};

RoundSetting_Private::~RoundSetting_Private()
{
	ResetEncoder();
}

void RoundSetting_Private::ResetEncoder()
{
	if(encoder != nullptr)
	{
        delete encoder;
	}
	encoder = nullptr;
}

bool RoundSetting_Private::CheckCanDo()
{
	return (*(uint64*)(pwd) != 0) && encoder != nullptr;
}

bool RoundSetting_Private::ByteEncode(void* dest, const char*src, uint64 length)
{
	return ByteEncode(encoder, dest, src, length);
}

bool RoundSetting_Private::ByteDecode(void* dest, const char*src, uint64 length)
{
	return ByteDecode(encoder, dest, src, length);
}

bool RoundSetting_Private::RowMix(void*dest, const char*src, uint64 length, bool isBack/* = false*/)
{
	uint8*pdest = static_cast<uint8*>(dest);
	static uint8 len[4][4] = {2,3,1,1,1,2,3,1,1,1,2,3,3,1,1,2};
	static uint8 backLen[4][4] = {14,11,13,9,9,14,11,13,13,9,14,11,11,13,9,14};
	if(isBack)
	{
		memcpy(len, backLen, 16);
	}
	for(uint64 i = 0; i + 16 < length; i += 16)
	{
		for(uint8 j = 0; j < 4; j++)
		{
			for(uint8 k = 0; k < 4; k++)
			{
				pdest[i + j * 4 + k] = BinMul(src[i + j * 4], len[0][k]) ^ BinMul(src[i + j * 4 + 1], len[1][k]) ^ BinMul(src[i + j * 4 + 2], len[2][k]) ^ BinMul(src[i + j * 4 + 3], len[3][k]);
			}
		}
	}
	return true;
}


bool RoundSetting_Private::Lock(void*dest, const char*src, uint64 length)
{
	return Lock(dest, src, length, pwd);
}


bool RoundSetting_Private::Lock(void*dest, const char*src, uint64 length, uint8 pwd[16])
{
	uint8*pdest = static_cast<uint8*>(dest);
	for(uint64 i = 0; i + 16 < length; i += 16)
	{
		for(int j = 0; j < 16; j++)
		{
			pdest[i + j] = src[i + j] ^ pwd[j];
		}
	}
	return true;
}

uint8 RoundSetting_Private::BinMul(uint8 x, uint8 y)
{
	if(y % 2 == 0)
	{
		return uint8(x << (y / 2));
	}
	return x ^ uint8(x << (y / 2));
}

bool RoundSetting_Private::LineMove(uint8 rectWidth, void*dest, const char*src, uint64 length, bool isBack /*= false*/)
{
	auto GetRound = [=](uint8 line, uint8 num)->uint8
	{
		if(line + num >= rectWidth)
			return uint8(num + line - rectWidth);
		else
			return uint8(num + line);
	};
	auto GetBack = [=](uint8 line, uint8 num)->uint8
	{
		if(line + num < 0)
			return uint8(num - line + rectWidth);
		else
			return uint8(num - line);
	};
	uint8*pdest = static_cast<uint8*>(dest);
	uint64 nowPos = 0;
	for(nowPos = 0; nowPos + rectWidth*rectWidth < length; nowPos += rectWidth*rectWidth)
	{
		for(uint8 j = 0; j < rectWidth; j++)
		{
			for(uint8 k = 0; k < rectWidth; k++)
			{
				pdest[nowPos + j*rectWidth + k] = src[nowPos + j*rectWidth + (isBack ? GetBack(j, k) : GetRound(j, k))];
			}
		}
	}
	return true;
}

bool RoundSetting_Private::LineMove(void*dest, const char*src, uint64 length)
{
	return LineMove(rectWidth, dest, src, length);
}

bool RoundSetting_Private::LineMoveBack(void*dest, const char*src, uint64 length)
{
	return LineMove(rectWidth, dest, src, length, true);
}

bool RoundSetting_Private::ByteEncode(ByteEncoder_Private_Ref* encoder, void* dest, const char*src, uint64 length)
{
	auto bhd = encoder->reffer;
	uint8* pdest = static_cast<uint8*>(dest);
	for(uint64 i = 0; i < length; i++)
	{
		pdest[i] = bhd->data[src[i]];
	}
	return true;
}

bool RoundSetting_Private::ByteDecode(ByteEncoder_Private_Ref* encoder, void* dest, const char*src, uint64 length)
{
	auto bhd = new ByteEncoder_Private_Ref();
	memcpy(bhd->reffer->data, encoder->reffer->data, 256);
    bhd->reffer->TurnToBack();
	bool ret = ByteEncode(bhd, dest, src, length);
    delete bhd;
	return ret;
}

RoundSetting::RoundSetting()
{
    AA_ROUND_SETTING_HANDLE_MANAGER.GetHandle(this);
}

RoundSetting::RoundSetting(const RoundSetting&setting)
{
    AA_ROUND_SETTING_HANDLE_MANAGER.GetHandle(this, new RoundSetting_Private_Ref(AA_ROUND_SETTING_HANDLE_MANAGER[&setting]->reffer));
}

RoundSetting::~RoundSetting()
{
    delete AA_ROUND_SETTING_HANDLE_MANAGER. ReleaseHandle(this);
}

bool RoundSetting::SetRoundPassword(const uint8 pwd[16])
{
	AAAssert(pwd != nullptr, false);
	memcpy(AA_ROUND_SETTING_HANDLE_MANAGER[this]->reffer->pwd, pwd, 16);
	return true;
}

bool RoundSetting::SetByteEncoder(const ByteEncoder&bEncoder)
{
	if(!AA_BYTE_ENCODER_HANDLE_MANAGER[&bEncoder]->reffer->CheckObayRule())
		return false;
	auto hd = AA_ROUND_SETTING_HANDLE_MANAGER[this]->reffer;
	hd->ResetEncoder();
	hd->encoder = new ByteEncoder_Private_Ref(AA_BYTE_ENCODER_HANDLE_MANAGER[&bEncoder]->reffer);
	return true;
}

bool RoundSetting::SetLineMoving(uint8 rectWidth /*= 4*/)
{
	if(rectWidth < 4)
		return false;
	AA_ROUND_SETTING_HANDLE_MANAGER[this]->reffer->rectWidth = rectWidth;
	return true;
}

ByteEncoder RoundSetting::GetByteEncoder()
{
    return *AA_BYTE_ENCODER_HANDLE_MANAGER.GetSourceByHandle(AA_ROUND_SETTING_HANDLE_MANAGER[this]->reffer->encoder);
}


const ByteEncoder RoundSetting::GetByteEncoder() const
{
    return *AA_BYTE_ENCODER_HANDLE_MANAGER.GetSourceByHandle(AA_ROUND_SETTING_HANDLE_MANAGER[this]->reffer->encoder);
}

uint8 RoundSetting::GetLineMoving() const
{
	return AA_ROUND_SETTING_HANDLE_MANAGER[this]->reffer->rectWidth;
}

bool RoundSetting::Encode(void* dest, const char*src, uint64 length, bool withRowMix /*= true*/)
{
	auto hd = AA_ROUND_SETTING_HANDLE_MANAGER[this]->reffer;
	if(dest == nullptr || src == nullptr || length < hd->rectWidth*hd->rectWidth)
		return false;
	if(!hd->ByteEncode(dest, src, length))
		return false;
	if(!hd->LineMove(dest, src, length))
		return false;
	if(withRowMix&&!hd->RowMix(dest, src, length))
		return false;
	return hd->Lock(dest, src, length);
}

bool RoundSetting::Decode(void* dest, const char*src, uint64 length, bool withRowMix /*= true*/)
{
	auto hd = AA_ROUND_SETTING_HANDLE_MANAGER[this]->reffer;
	if(dest == nullptr || src == nullptr || length < 16)
		return false;
	if(!hd->Lock(dest, src, length))
		return false;
	if(!withRowMix&&!hd->RowMix(dest, src, length, true))
		return false;
	if(!hd->LineMoveBack(dest, src, length))
		return false;
	return hd->ByteDecode(dest, src, length);
}

/********************* Code : Parser ********************************************************************/

class Parser_Private
{
public:
    ~Parser_Private();

public:
	std::vector<RoundSetting_Private_Ref*> settings;
	void* data = nullptr;
	uint64 length = 0;
	uint8 fpwd[16] = {0};

	uint16 refCount = 1;

public:
	inline static uint32 GetGPwd(uint32 src, uint8 rank);
};

class Parser_Private_Ref
{
public:
    Parser_Private_Ref(Parser_Private* reffer = nullptr)
    {
        if (reffer == nullptr)
            this->reffer = new Parser_Private();
        else
        {
            this->reffer = reffer;
            ++(reffer->refCount);
        }
    }
    ~Parser_Private_Ref()
    {
        --(reffer->refCount);
        if (reffer->refCount <= 0)
            delete reffer;
    }
    Parser_Private&operator ->()
    {
        return *reffer;
    }
    Parser_Private&operator*()
    {
        return *reffer;
    }
    Parser_Private* reffer = nullptr;
};

Parser_Private::~Parser_Private()
{
    for (auto i = settings.begin(); i != settings.end(); ++i)
        delete*i;
}

uint32 Parser_Private::GetGPwd(uint32 src, uint8 rank)
{
	AAAssert(rank > 3 && rank < 44, 0U);
	if(src >> 31 > 0)
		src = (src << 1) + 1;
	else src = src << 1;
	uint8 tmp[4] = {0};
	memcpy(tmp, &src, 4);
	static uint8 len[4][4] = {2,3,1,1,1,2,3,1,1,1,2,3,3,1,1,2};
	for(uint8 k = 0; k < 4; k++)
	{
		tmp[k] = RoundSetting_Private::BinMul(uint8(src / 65536 / 256), len[0][k]) ^ RoundSetting_Private::BinMul(uint8(src / 65536 % 256), len[1][k]) ^ RoundSetting_Private::BinMul(uint8(src / 256 % 65536), len[2][k]) ^ RoundSetting_Private::BinMul(uint8(src % 256), len[3][k]);
	}
	src = tmp[0] * 65536 * 256 + tmp[1] * 65536 + tmp[2] * 256 + tmp[3];
	static uint8 RC[] = {0, 1, 2, 4, 8, 16, 32, 64, 128, 127, 54};
	return src ^ (RC[rank / 4]);
}

Parser::Parser()
{
    AA_PARSER_HANDLE_MANAGER.GetHandle(this);
}


Parser::Parser(const Parser&value)
{
    AA_PARSER_HANDLE_MANAGER.GetHandle(this, new Parser_Private_Ref(AA_PARSER_HANDLE_MANAGER[&value]->reffer));
}

Parser::~Parser()
{
    delete AA_PARSER_HANDLE_MANAGER. ReleaseHandle(this);
}


bool Parser::SetFirstlyPwd(uint8 pwd[16])
{
	memcpy(AA_PARSER_HANDLE_MANAGER[this]->reffer->fpwd, pwd, 16);
	return true;
}

bool Parser::SetRounds(const RoundSetting settingArray[], int roundsCount)
{
	auto hd = AA_PARSER_HANDLE_MANAGER[this]->reffer;
	for(int i = 0; i < roundsCount; i++)
	{
		if(AA_ROUND_SETTING_HANDLE_MANAGER[&settingArray[i]] != nullptr)
		{
			hd->settings.push_back(new RoundSetting_Private_Ref(AA_ROUND_SETTING_HANDLE_MANAGER[&settingArray[i]]->reffer));
		}
	}
	return true;
}


bool Parser::SetRound(uint8 round, const RoundSetting setting)
{
    delete AA_PARSER_HANDLE_MANAGER[this]->reffer->settings[round];
    AA_PARSER_HANDLE_MANAGER[this]->reffer->settings[round] = new RoundSetting_Private_Ref(AA_ROUND_SETTING_HANDLE_MANAGER[&setting]->reffer);
	return true;
}


bool Parser::SetData(void*data, uint64 length)
{
	auto hd = AA_PARSER_HANDLE_MANAGER[this]->reffer;
	hd->data = data;
	hd->length = length;
	return true;
}


RoundSetting Parser::GetSetting(uint8 round) const
{
    auto ret = AA_PARSER_HANDLE_MANAGER[this]->reffer->settings[round]->reffer;

	AAAssert(false, *(RoundSetting*)(nullptr));
	/* 因 ClassPrivateHandleManager 安全性修正, 此处代码已不可用, 若稍后要用到此处功能, 另行寻找解决办法
    for (auto i = AA_ROUND_SETTING_HANDLE_MANAGER.handleMap.begin(); i != AA_ROUND_SETTING_HANDLE_MANAGER.handleMap.end(); ++i)
    {
        if (i->second->reffer == ret)
            return *i->first;
    }
	*/
    return *(RoundSetting*)(nullptr);
}


uint8 Parser::GetRoundCount() const
{
	return uint8(AA_PARSER_HANDLE_MANAGER[this]->reffer->settings.size());
}


bool Parser::Encode(void*dest, void*data /*= nullptr*/, uint64 length /*= 0*/)
{
	auto hd = AA_PARSER_HANDLE_MANAGER[this]->reffer;
	if(!RoundSetting_Private::Lock(dest, static_cast<char*>(data), length, hd->fpwd))
		return false;
	auto rounds = GetRoundCount();
	if(data == nullptr)
		data = hd->data;
	if(length == 0)
		length = hd->length;
	for(uint8 i = 0; i < rounds - 1; i++)
	{
		if(!GetSetting(i).Encode(dest, static_cast<char*>(data), length))
			return false;
	}
	return GetSetting(rounds-1).Encode(dest, static_cast<char*>(data), length, false);
}


bool Parser::Decode(void*dest, void*data /*= nullptr*/, uint64 length /*= 0*/)
{
	auto hd = AA_PARSER_HANDLE_MANAGER[this]->reffer;
	auto rounds = GetRoundCount();
	if(data == nullptr)
		data = hd->data;
	if(length == 0)
		length = hd->length;
	if(!GetSetting(rounds - 1).Decode(dest, static_cast<char*>(data), length, false))
		return false;
	for(int i = rounds - 2; i >= 0; i--)
	{
		if(!GetSetting(i).Decode(dest, static_cast<char*>(data), length))
			return false;
	}
	return RoundSetting_Private::Lock(dest, static_cast<char*>(data), length, hd->fpwd);
}


bool Parser::GetExtendPwds(uint8 initPwd[16], uint8 gettedPwd[176])
{
	uint32 passwords[44];
	for(int i = 0; i < 4; i++)
	{
		passwords[i] = initPwd[i] * 65536 * 256 + initPwd[i + 4] * 65536 + initPwd[i + 8] * 256 + initPwd[i + 12];
	}
	for(uint8 i = 4; i < 44; i++)
	{
		if(i % 4 == 0)
			passwords[i] = passwords[i - 4] ^ Parser_Private::GetGPwd(passwords[uint8(i - 1)], i);
		else
			passwords[i] = passwords[i - 4] ^ passwords[i - 1];
	}
	memcpy(gettedPwd, passwords, 176);
	return true;
}


Parser Parser::GetQuickParser(uint8 initPwd[16], uint8 byteEncoder[256] /*= nullptr*/)
{
	Parser ret;
	uint8 pwds[176] = {0};
	AAAssert(GetExtendPwds(initPwd, pwds), ret);
	AAAssert(ret.SetFirstlyPwd(pwds), ret);
	RoundSetting setting[10];
	ByteEncoder bencoder;
	if(byteEncoder == nullptr)
		bencoder = ByteEncoder::GetRandomEncoder();
	else
		AAAssert(bencoder.InputData(byteEncoder, true), ret);
	for(int i = 0; i < 10; i++)
	{
		AAAssert(setting[i].SetByteEncoder(bencoder), ret);
		AAAssert(setting[i].SetRoundPassword(pwds + 16 + 16 * i), ret);
	}
	AAAssert(ret.SetRounds(setting, 10), ret);
	return ret;
	//这里返回时会不会导致数据被销毁？需要谨慎定义赋值运算符和拷贝构造函数
}

}

}

#undef AA_BYTE_ENCODER_HANDLE_MANAGER
#undef AA_ROUND_SETTING_HANDLE_MANAGER
#undef AA_PARSER_HANDLE_MANAGER
