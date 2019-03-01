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

#include <cmath>
#include "../../inc/AAString.h"
#include "../../inc/AAClassPrivateHandle.hpp"
#include <sstream>
#include <vector>

#ifdef OS_BSD
#include <cmath>
#endif


#define AA_HANDLE_MANAGER ClassPrivateHandleManager<String, std::string>::getInstance()

namespace ArmyAnt{


String::String(const char * value){
	if(value == nullptr)
		value = "";
	auto newStr = new std::string(value);
	AA_HANDLE_MANAGER.GetHandle(this, newStr);
	resetValue();
}

String::String(char c){
	AA_HANDLE_MANAGER.GetHandle(this, new std::string(c + std::string()));
	resetValue();
}

String::String(int32 num){
	std::ostringstream tmp;
	tmp << num;
	AA_HANDLE_MANAGER.GetHandle(this, new std::string(tmp.str()));
	resetValue();
}

String::String(const int64 & num){
	std::ostringstream tmp;
	tmp << num;
	AA_HANDLE_MANAGER.GetHandle(this, new std::string(tmp.str()));
	resetValue();
}

String::String(double num, int32 behindFloat){
	if(behindFloat >= 0){
		int64 powed = int64(pow(10, behindFloat));
		num -= num - double(int64(num*powed)) / powed;
	}
	std::ostringstream tmp;
	tmp << num;
	AA_HANDLE_MANAGER.GetHandle(this, new std::string(tmp.str()));
	resetValue();
}

String::String(const String & value){
	AA_HANDLE_MANAGER.GetHandle(this, new std::string(*(AA_HANDLE_MANAGER[&value])));
	resetValue();
}

String::String(String && _moved){
	AA_HANDLE_MANAGER.MoveTo(&_moved, this);
	resetValue();
}

String::~String(){
	if(AA_HANDLE_MANAGER.GetDataByHandle(this, true) != nullptr){

		auto hd = AA_HANDLE_MANAGER.ReleaseHandle(this);
		delete hd;
	}
}

bool String::empty() const{
	return AA_HANDLE_MANAGER[this]->empty();
}

const char * String::c_str() const{
	return AA_HANDLE_MANAGER[this]->c_str();
}

uint64 String::size() const{
	return AA_HANDLE_MANAGER[this]->size();
}

bool String::isNumeric() const{
	auto ret = (*AA_HANDLE_MANAGER[this]);
	if(atof(ret.c_str()) == 0.0)
		for(size_t i = 0; i < ret.size(); ++i){
			if(ret[i] != ' ' && ret[i] != '0' && ret[i] != '.')
				return false;
		}
	return true;
}

bool String::isFloat() const{
	if(!isNumeric())
		return false;
	if(find('.') == c_npos)
		return false;
	return true;
}

int32 String::toInteger() const{
	return atoi(AA_HANDLE_MANAGER[this]->c_str());
}

int64 String::toLong() const{
	return atoll(AA_HANDLE_MANAGER[this]->c_str());
}

double String::toDemical() const{
	return atof(AA_HANDLE_MANAGER[this]->c_str());
}

char String::getChar(int32 index) const{
	if(index < 0)
		index += AA_HANDLE_MANAGER[this]->size();
	return (*(AA_HANDLE_MANAGER[this]))[index];
}

String & String::operator=(const String & value){
	*(AA_HANDLE_MANAGER[this]) = value.c_str();
	resetValue();
	return *this;
}

String & String::operator=(const char * value){
	*(AA_HANDLE_MANAGER[this]) = value;
	resetValue();
	return *this;
}

String & String::operator=(char c){
	*(AA_HANDLE_MANAGER[this]) = String() + c;
	resetValue();
	return *this;
}

String & String::operator=(int64 value){
	std::ostringstream ss;
	ss << value;
	*(AA_HANDLE_MANAGER[this]) += ss.str();
	resetValue();
	return *this;
}

String & String::operator=(double value){
	std::ostringstream ss;
	ss << value;
	*(AA_HANDLE_MANAGER[this]) += ss.str();
	resetValue();
	return *this;
}

bool String::operator<(const String & value) const{
	auto hdThis = AA_HANDLE_MANAGER[this];
	auto hdValue = AA_HANDLE_MANAGER[&value];
	return *hdThis < *hdValue;
}

bool String::operator==(const String & value) const{
	return *(AA_HANDLE_MANAGER[this]) == *(AA_HANDLE_MANAGER[&value]);
}

bool String::operator!=(const String & value) const{
	return !operator==(value);
}

bool String::operator==(const char * value) const{
	return *(AA_HANDLE_MANAGER[this]) == value;
}

bool String::operator!=(const char * value) const{
	return !operator==(value);
}

String::operator const char*()const{
	return c_str();
}

bool String::operator!() const{
	return empty();
}

String String::operator+(const String & value) const{
	return String((*(AA_HANDLE_MANAGER[this]) + *(AA_HANDLE_MANAGER[&value])).c_str());
}

String String::operator+(const char * value) const{
	return String((*(AA_HANDLE_MANAGER[this]) + value).c_str());
}

String String::operator+(char c) const{
	return String((*(AA_HANDLE_MANAGER[this]) + c).c_str());
}

String String::operator+(int64 value) const{
	std::ostringstream ss;
	ss << *AA_HANDLE_MANAGER[this];
	ss << value;
	return String(ss.str().c_str());
}

String String::operator+(double value) const{
	std::ostringstream ss;
	ss << *AA_HANDLE_MANAGER[this];
	ss << value;
	return String(ss.str().c_str());
}

String & String::operator+=(const String & value){
	*(AA_HANDLE_MANAGER[this]) += *(AA_HANDLE_MANAGER[&value]);
	resetValue();
	return *this;
}

String & String::operator+=(const char * value){
	*(AA_HANDLE_MANAGER[this]) += value;
	resetValue();
	return *this;
}

String & String::operator+=(char c){
	*(AA_HANDLE_MANAGER[this]) += c;
	resetValue();
	return *this;
}

String & String::operator+=(int64 value){
	std::ostringstream ss;
	ss << value;
	*(AA_HANDLE_MANAGER[this]) += ss.str();
	resetValue();
	return *this;
}

String & String::operator+=(double value){
	std::ostringstream ss;
	ss << value;
	*(AA_HANDLE_MANAGER[this]) += ss.str();
	resetValue();
	return *this;
}

String operator+(char c, const String & str){
	return str + c;
}

bool String::clearFront(const char ** value, uint32 length){
	auto hddd = AA_HANDLE_MANAGER[this];
	auto& str = *(hddd);
	if(value == nullptr || length == 0 || str.empty())
		return false;
	int first = 0;
	while(first < str.size()){
		bool isSame = false;
		for(uint32 i = 0; i < length; ++i){
			if(value[i] != nullptr){
				auto strLen = strlen(value[i]);
				if(value[i][strLen - 1] == '\0')
					strLen--;
				bool isThisSame = true;
				for(size_t k = 0; k < strLen; ++k){
					if(*(str.c_str() + first + k) != value[i][k]){
						isThisSame = false;
						break;
					}
				}
				if(isThisSame){
					isSame = true;
					break;
				}
			}
		}
		if(!isSame)
			break;
		++first;
	}
	return subString(first);
}

bool String::clearBack(const char ** value, uint32 length){
	auto& str = *(AA_HANDLE_MANAGER[this]);
	if(value == nullptr || length == 0 || str.empty())
		return false;
	int last = str.size();
	std::vector<size_t> strlens;
	for(uint32 i = 0; i < length; ++i){
		strlens.push_back(strlen(value[i]));
	}
	while(last >= 0){
		bool isSame = false;
		for(uint32 i = 0; i < length; ++i){
			if(value[i] != nullptr){
				auto strLen = strlen(value[i]);
				if(value[i][strLen - 1] == '\0')
					strLen--;
				bool isThisSame = true;
				for(size_t k = 0; k < strLen; ++k){
					if(*(str.c_str() + last - strlens[i] + k) != value[i][k]){
						isThisSame = false;
						break;
					}
				}
				if(isThisSame){
					isSame = true;
					break;
				}
			}
		}
		if(!isSame)
			break;
		--last;
	}
	return subString(int64(0), last);
}

bool String::clearBothSides(const char ** value, uint32 length){
	return clearFront(value, length) && clearBack(value, length);
}

bool String::subString(int64 start){
	return subString(start, int64(0));
}

bool String::subString(int64 start, int64 end){
	auto& str = *(AA_HANDLE_MANAGER[this]);
	auto size = str.size();
	if(start < 0)
		start += size;
	if(end <= 0)
		end += size;
	if(start < 0 && end < 0 && start > end)
		return false;
	if(end < size)
		str[end] = '\0';
	std::string tmp = str.c_str() + start;
	str = tmp.c_str();
	resetValue();
	return true;
}

int32 String::find(char c) const{
	auto& str = *(AA_HANDLE_MANAGER[this]);
	auto ret = str.find(c);
	if(ret == str.npos)
		return c_npos;
	return int32(ret);
}

String operator+(const char*value, const String&str){
	return String(value) + str;
}

bool operator==(const char*cstr, const String&str){
	return str == cstr;
}


bool String::replace(char src, const char * tar){
	auto& str = *(AA_HANDLE_MANAGER[this]);
	auto len = str.length();
	for(size_t i = 0; i < len; ++i){
		if(str[i] == src){
			str = str.replace(i, 1, tar);
			auto newLen = str.length();
			i += newLen - len;
			len = newLen;
		}
	}
	resetValue();
	return true;
}

void String::resetValue(){
#if defined _DEBUG && defined OS_WINDOWS
	v = AA_HANDLE_MANAGER[this]->c_str();
#endif
}

}

#undef AA_HANDLE_MANAGER
