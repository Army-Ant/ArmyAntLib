/* Copyright (c) 2015 ArmyAnt
 * ��Ȩ���� (c) 2015 ArmyAnt
 *
 * Licensed under the BSD License, Version 2.0 (the License);
 * ������ʹ��BSDЭ�鱣��, Э��汾:2.0
 * you may not use this file except in compliance with the License.
 * ʹ�ñ���Դ�����ļ�������, ��Ϊͬ��Э��
 * You can read the license content in the file "LICENSE" at the root of this project
 * �������ڱ���Ŀ�ĸ�Ŀ¼�ҵ���Ϊ"LICENSE"���ļ�, ���Ķ�Э������
 * You may also obtain a copy of the License at
 * ��Ҳ�����ڴ˴����Э��ĸ���:
 *
 *     http://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * ���Ƿ���Ҫ����߰�Ȩ����������ͬ��,�������ڱ�Э������ϵķ���û���κ���ʽ�������͵���,������ʾ�Ļ�Ĭ����.
 * See the License for the specific language governing permissions and limitations under the License.
 * �����ض����ƻ����Թ���Ȩ�����Ķ�Э��
 * This file is the internal source file of this project, is not contained by the closed source release part of this software
 * ���ļ�Ϊ�ڲ�Դ���ļ�, ��������ڱ�Դ�����ı�������
 */

#include "../../inc/AATimeUtilities.h"

#include <ctime>
#ifdef OS_LINUX
#include <sys/time.h>
#endif

namespace ArmyAnt{

namespace TimeUtilities{

ArmyAnt::String getTimeStamp(){
#if defined OS_WINDOWS
	auto tm = _time64(0);
	char ret[64] = "";
	_ctime64_s(ret, &tm);
	auto retStr = ArmyAnt::String(ret);
	retStr.replace('\n', "");
#else
	auto tm = time(0);
	auto retStr = ArmyAnt::String(ctime(&tm));
#endif
	return retStr;
}

int64 getCurrentTime(){
#if defined OS_WINDOWS
	auto tm = _time64(0);
	char ret[64] = "";
	_ctime64_s(ret, &tm);
	return int64(tm);
#else
	auto tm = time(0);
	return int64(ctime(&tm));
#endif
}

}

}