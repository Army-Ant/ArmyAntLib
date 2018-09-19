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

#ifndef C_AA_FILE_H_2015_11_6
#define C_AA_FILE_H_2015_11_6

#include "C_AADefine.h"
#include "AA_start.h"

#ifdef _cplusplus
extern "C" {
#endif

	typedef enum
	{
		None,
		File,
		Memory,
		NamePipe,
		ComData,
		Network
	} AA_StreamType;

	typedef mac_uint AA_CStream;

	ARMYANT_CLIB_API AA_CStream AA_Stream_CreateByUrl(const char* url);
	ARMYANT_CLIB_API AA_CStream AA_Stream_CreateByType(AA_StreamType type, const char* src);
	ARMYANT_CLIB_API AA_CStream AA_File_Create();
	ARMYANT_CLIB_API AA_CStream AA_Memory_Create();
	ARMYANT_CLIB_API void AA_Stream_Release(AA_CStream stream);

	ARMYANT_CLIB_API BOOL AA_Stream_Open(AA_CStream stream, const char* src);
	ARMYANT_CLIB_API BOOL AA_Stream_Close(AA_CStream stream);
	ARMYANT_CLIB_API BOOL AA_Stream_IsOpened(AA_CStream stream);
	ARMYANT_CLIB_API BOOL AA_Stream_IsOpening(AA_CStream stream);
	ARMYANT_CLIB_API const char* AA_Stream_GetStreamName(AA_CStream stream);

	ARMYANT_CLIB_API mac_int AA_StaticStream_GetLength(AA_CStream stream);
	ARMYANT_CLIB_API mac_int AA_StaticStream_GetPos(AA_CStream stream);
	ARMYANT_CLIB_API BOOL AA_StaticStream_IsEndPos(AA_CStream stream);
	ARMYANT_CLIB_API BOOL AA_StaticStream_MovePos(AA_CStream stream, uint32 pos);
	ARMYANT_CLIB_API mac_int AA_StaticStream_ReadSome(AA_CStream stream, void*buffer, uint32 len, uint32 pos);
	ARMYANT_CLIB_API mac_int AA_StaticStream_ReadTo(AA_CStream stream, void*buffer, uint8 endtag, uint32 maxlen);
	ARMYANT_CLIB_API mac_int AA_StaticStream_Write(AA_CStream stream, void*buffer, uint32 len);

	ARMYANT_CLIB_API BOOL AA_File_SetMode(AA_CStream stream, BOOL nocreate, BOOL noexist);

	ARMYANT_CLIB_API BOOL AA_File_CopyFile(const char*srcPath, const char*dstPath);
	ARMYANT_CLIB_API BOOL AA_File_MoveOrRenameFile(const char*srcPath, const char*dstPath);
	ARMYANT_CLIB_API BOOL AA_File_DeleteFile(const char*path);
	ARMYANT_CLIB_API BOOL AA_File_IsFileExist(const char*path);

#ifdef _cplusplus
}
#endif

#endif
