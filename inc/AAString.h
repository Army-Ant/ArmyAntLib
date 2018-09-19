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
 */

#ifndef AA_STRING_H_2017_4_5
#define AA_STRING_H_2017_4_5

#include "AA_start.h"
#include "AADefine.h"
#include <iostream>
#include <cstring>

namespace ArmyAnt
{

enum class Encoding
{
    ANSI,
    EBCDIC,
    UTF8,
    UTF16,
    UTF32,
    UTF7
};

class ARMYANTLIB_API String
{
public:
    String(const char*value = nullptr);
    String(char c);
    String(int32 num);
    String(const int64&num);
    String(double num, int32 behindFloat = -1);
    String(const String&value);
    String(String&&_moved);
    ~String();

public:
    bool empty()const;
    const char* c_str()const;
    uint64 size()const;
    bool isNumeric()const;
    bool isFloat()const;
    int32 toInteger()const;
    int64 toLong()const;
    double toDemical()const;
    Encoding getEncoding()const;

public:
    bool encode(Encoding encode);
    void clear();
    String& upsideDown();
    uint64 copyTo(char*dest, uint64 maxLength = 0);

    int32 find(char c)const;
    int32 find(const char*str)const;
    static const int64 c_npos = -1;

    bool clearFront(char c);
    bool clearFront(const String&value);
    bool clearFront(const char*value);
    bool clearFront(const char**value, uint32 length);

    bool clearBack(char c);
    bool clearBack(const String&value);
    bool clearBack(const char*value);
    bool clearBack(const char**value, uint32 length);

    bool clearBothSides(char c);
    bool clearBothSides(const String&value);
    bool clearBothSides(const char*value);
    bool clearBothSides(const char**value, uint32 length);

    bool clearAnywhere(char c);
    bool clearAnywhere(const String&value);
    bool clearAnywhere(const char*value);
    bool clearAnywhere(const char**value, uint32 length);

    bool replace(char src, char tar);
    bool replace(char src, const String&tar);
    bool replace(char src, const char*tar);
    bool replace(const String&src, char tar);
    bool replace(const String&src, const String&tar);
    bool replace(const String&src, const char*tar);
    bool replace(const char*src, char tar);
    bool replace(const char*src, const String&tar);
    bool replace(const char*src, const char*tar);

    bool subString(int64 start);
    bool subString(int64 start, int64 end);
    bool subString(int64 start, const char* endstr);
    bool subString(int64 start, const String& endstr);
    bool subString(const char* start);
    bool subString(const char* start, int64 end);
    bool subString(const char* start, const char* endstr);
    bool subString(const char* start, const String& endstr);
    bool subString(const String& start);
    bool subString(const String& start, int64 end);
    bool subString(const String& start, const char* endstr);
    bool subString(const String& start, const String& endstr);

    String getReplaced(char src, char tar)const;
    String getReplaced(char src, const String&tar)const;
    String getReplaced(char src, const char*tar)const;
    String getReplaced(const String&src, char tar)const;
    String getReplaced(const String&src, const String&tar)const;
    String getReplaced(const String&src, const char*tar)const;
    String getReplaced(const char*src, char tar)const;
    String getReplaced(const char*src, const String&tar)const;
    String getReplaced(const char*src, const char*tar)const;

    String getSubString(int64 start)const;
    String getSubString(int64 start, int64 end)const;
    String getSubString(int64 start, char endc)const;
    String getSubString(int64 start, const char* endstr)const;
    String getSubString(int64 start, const String& endstr)const;
    String getSubString(char start)const;
    String getSubString(char start, int64 end)const;
    String getSubString(char start, char endc)const;
    String getSubString(char start, const char* endstr)const;
    String getSubString(char start, const String& endstr)const;
    String getSubString(const char* start)const;
    String getSubString(const char* start, int64 end)const;
    String getSubString(const char* start, char endc)const;
    String getSubString(const char* start, const char* endstr)const;
    String getSubString(const char* start, const String& endstr)const;
    String getSubString(const String& start)const;
    String getSubString(const String& start, int end)const;
    String getSubString(const String& start, char endc)const;
    String getSubString(const String& start, const char* endstr)const;
    String getSubString(const String& start, const String& endstr)const;

	char getChar(int32 index)const;

public:
    // overloaded operators
    String& operator=(const String&value);
    String& operator=(const char*value);
    String& operator=(char c);
    String& operator=(int64 value);
    String& operator=(double value);
	bool operator<(const String&value)const;
    bool operator==(const String&value)const;
    bool operator!=(const String&value)const;
    bool operator==(const char*value)const;
    bool operator!=(const char*value)const;
	operator const char*()const;
    bool operator!()const;
    String operator+(const String&value)const;
    String operator+(const char*value)const;
    String operator+(char c)const;
    String operator+(int64 value)const;
    String operator+(double value)const;
    String& operator+=(const String&value);
    String& operator+=(const char*value);
    String& operator+=(char c);
    String& operator+=(int64 value);
    String& operator+=(double value);
    String operator-(int tailLength)const;
    String operator-(char c)const;
    String operator-(const String&value)const;
    String operator-(const char*value)const;
    String& operator-=(int tailLength);

public:
    static Encoding getEncoding(char*str);
    static bool encode(Encoding from, Encoding to, char*str);

    template <class Type_Num = int> 
    static inline bool itoa(char*str, Type_Num num);
    static bool isStringEmpty(const char*str);
    static bool isStringNumeric(const char*str);
    static int32 toInteger(const char*str);
    static int32 toDemical(const char*str);

    static bool clearFront(char*str, char c);
    static bool clearFront(char*str, const String&value);
    static bool clearFront(char*str, const char*value);
    static bool clearFront(char*str, const char**value, uint32 length);

    static bool clearBack(char*str, char c);
    static bool clearBack(char*str, const String&value);
    static bool clearBack(char*str, const char*value);
    static bool clearBack(char*str, const char**value, uint32 length);
 
    static bool clearBothSides(char*str, char c);
    static bool clearBothSides(char*str, const String&value);
    static bool clearBothSides(char*str, const char*value);
    static bool clearBothSides(char*str, const char**value, uint32 length);

    static bool replace(char*str, char src, char tar);
    static bool replace(char*str, char src, const String&tar);
    static bool replace(char*str, char src, const char*tar);
    static bool replace(char*str, const String&src, char tar);
    static bool replace(char*str, const String&src, const String&tar);
    static bool replace(char*str, const String&src, const char*tar);
    static bool replace(char*str, const char*src, char tar);
    static bool replace(char*str, const char*src, const String&tar);
    static bool replace(char*str, const char*src, const char*tar);
    static bool upsideDown(char*str);
    static bool upsideDown(char*dest, const char*src);

    static bool subString(char*str, int64 start);
    static bool subString(char*str, int64 start, int64 end);
    static bool subString(char*str, int64 start, char endc);
    static bool subString(char*str, int64 start, const char* endstr);
    static bool subString(char*str, int64 start, const String& endstr);
    static bool subString(char*str, char start);
    static bool subString(char*str, char start, int64 end);
    static bool subString(char*str, char start, char endc);
    static bool subString(char*str, char start, const char* endstr);
    static bool subString(char*str, char start, const String& endstr);
    static bool subString(char*str, const char* start);
    static bool subString(char*str, const char* start, int64 end);
    static bool subString(char*str, const char* start, char endc);
    static bool subString(char*str, const char* start, const char* endstr);
    static bool subString(char*str, const char* start, const String& endstr);
    static bool subString(char*str, const String& start);
    static bool subString(char*str, const String& start, int64 end);
    static bool subString(char*str, const String& start, char endc);
    static bool subString(char*str, const String& start, const char* endstr);
    static bool subString(char*str, const String& start, const String& endstr);

public:
    friend inline bool operator<<(std::iostream&stream, const String&str);

#if defined _DEBUG && defined OS_WINDOWS
private:
	const char* v;
#endif
	void resetValue();
};

String ARMYANTLIB_API operator+(const char*value, const String&str);
String ARMYANTLIB_API operator+(char c, const String&str);
String ARMYANTLIB_API operator+(int64 value, const String&str);
String ARMYANTLIB_API operator+(double value, const String&str);
String ARMYANTLIB_API operator-(int headLength, const String&str);
bool ARMYANTLIB_API operator==(const char*cstr, const String&str);
bool ARMYANTLIB_API operator!=(const char*cstr, const String&str);


template<class Type_Num>
inline bool String::itoa(char * str, Type_Num num)
{
    if (str == nullptr)
        return false;
    String ret = "";
    bool isNegative = num < 0;
    num = Fragment::abs(num);
    Type_Num afterPoint = Type_Num(num - uint64(num));
    Type_Num beforePoint = num - afterPoint;
    do
    {
        ret = (beforePoint % 10 + '0') + ret;
        beforePoint /= 10;
    }
    while (beforePoint != 0);
    if (num - 0.1f != num)
    {
        ret += ".";
        do
        {
            beforePoint *= 10;
            ret = ret + (char(afterPoint + '0'));
        }
        while (beforePoint != 0.0);
    }
    if (isNegative)
        ret = "-" + ret;
    strcpy(str, ret.c_str());
    return true;
}

}

#endif // AA_STRING_H_2017_4_5
