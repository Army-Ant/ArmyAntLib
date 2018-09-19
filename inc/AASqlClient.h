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

#ifndef AA_SQL_CLIENT_H_2017_4_1
#define AA_SQL_CLIENT_H_2017_4_1

#include "AASqlStructs.h"

namespace ArmyAnt
{

class ARMYANTLIB_API ISqlClient
{
public:
    ISqlClient();
    virtual ~ISqlClient();

public:
    virtual bool connect(const String&serverHost)=0;
    virtual bool connect(const String&serverAddress, const String&port) = 0;
    virtual void disconnect() = 0;
    virtual bool isConnection() = 0;
    virtual uint32 getDatabaseCount() = 0;
    virtual uint32 getDatabaseList(String*&dbs, uint32 maxCount = 0) = 0;

public:
	virtual int64 getTablesCount()=0;
	virtual int64 getViewsCount()=0;
	virtual int64 getTableNameList(ArmyAnt::String*&tables, uint32 maxCount = 0)=0;
	virtual int64 getViewNameList(ArmyAnt::String*&views, uint32 maxCount = 0)=0;
    virtual SqlTable getWholeTable(const String&tableName);
    virtual SqlTable getWholeView(const String&tableName);
	virtual int64 getTableAllFields(const ArmyAnt::String&table, ArmyAnt::String*&fields, uint32 maxCount = 0) = 0;

public:
    // select * from [tableName]
    virtual SqlTable select(const String&tableName, const SqlClause*clauses = nullptr, int clausesNum = 0);
    // select [columnNames] from [tableName]
    virtual SqlTable select( const String&tableName, const String*columnNames, int columnNum, const SqlClause*clauses = nullptr, int clausesNum = 0);
    // update [tableName] set [updatedData ( k=value , k=value ... )]
    virtual int64 update(const String&tableName, const SqlRow&updatedData, const SqlClause*clauses = nullptr, int clausesNum = 0);
    // insert into [tableName] [insertedData (k , k , k ... ) values ( value , value , value ... )]
    virtual int64 insertRow(const String&tableName, const SqlRow&insertedData);
    // alter table [tableName] add [columnHead name dataType (others)...]
    virtual int64 insertColumn(const String&tableName, const SqlFieldHead&columnHead);
    virtual int64 insertColumn(const String&tableName, const SqlColumn&column);
    // delete from [tableName]
    virtual int64 deleteRow(const String&tableName, const SqlClause*where = nullptr);
    // alter table [tableName] drop column [columnName]
    virtual int64 deleteColumn(const String&tableName, const String&columnName);

public:
    virtual int64 createDatabase(const String&dbName);
    virtual int64 deleteDatabase(const String&dbName);
    virtual int64 createTable(const String&tableName, const SqlColumn&column, const SqlTableInfo*tableInfo = nullptr);
    virtual int64 deleteTable(const String&tableName);

public:
    virtual String organizeColumnInfo(const SqlFieldHead&column);
    virtual String organizeSqlClause(const SqlClause*clauses = nullptr, int clausesNum = 0);
    virtual SqlTable query(const String&sql) = 0;
    virtual int64 update(const String&sql) = 0;
	// Call 'excute' to execute multi-result sql, if the result param was run, the sql will still be executed
	// 调用驱动的execute执行可以返回多个结果集的SQL语句, 如果传入的result参数不可用, sql语句仍将被执行, 且结果仍将返回结果集个数. 
	// 只有sql语句执行失败才会返回负数. 如果执行update语句, 则不会返回任何结果, 但返回值会返回影响的行数 (同update)
    virtual int execute(const String&sql, SqlTable*result, int32 maxResultCount) = 0;

protected:
	static SqlTable createSqlTable(const SqlFieldHead* heads, uint32 width, uint32 height);

    AA_FORBID_ASSGN_OPR(ISqlClient);
    AA_FORBID_COPY_CTOR(ISqlClient);
};

}

#endif // AA_SQL_CLIENT_H_2017_4_1