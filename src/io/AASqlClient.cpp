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

#include "../../inc/AASqlClient.h"

namespace ArmyAnt {

    ISqlClient::ISqlClient() {
    }

    ISqlClient::~ISqlClient() {
    }

    SqlTable ISqlClient::getWholeTable(const String &tableName) {
        return select( tableName);
    }

    SqlTable ISqlClient::getWholeView(const String &tableName) {
        return select( tableName);
    }

    SqlTable
    ISqlClient::select(const String &tableName, const SqlClause *clauses, int clausesNum) {
        return query("select * from " + tableName + organizeSqlClause(clauses, clausesNum));
    }

    SqlTable ISqlClient::select(const String &tableName, const String *columnNames, int columnNum,
                                const SqlClause *clauses, int clausesNum) {
        String sql = "select ";
        if (columnNames != nullptr)
            for (int i = 0; i < columnNum; ++i) {
                sql += columnNames[i] + " , ";
            }
        return query(sql + "from " + tableName + organizeSqlClause(clauses, clausesNum));
    }

	int64 ISqlClient::update(const String &tableName, const SqlRow &updatedData,
                            const SqlClause *clauses, int clausesNum) {
        if (updatedData.size() <= 0)
            return -1;
        String sql = "update " + tableName + " set ";
        for (uint32 i = 0; i < updatedData.size(); ++i) {
            sql += updatedData[i].getHead()->columnName + " = \"" + updatedData[i].getValue() + "\" ";
            if (i != updatedData.size() - 1)
                sql += ", ";
        }
        return update(sql + organizeSqlClause(clauses, clausesNum));
    }

	int64 ISqlClient::insertRow(const String &tableName, const SqlRow &insertedData) {
        if (insertedData.size() <= 0)
            return -1;
        String keys = "";
        String values = "";
        for (uint32 i = 0; i < insertedData.size(); ++i) {
            keys += insertedData[i].getHead()->columnName;
            values += insertedData[i].getValue();
            if (i != insertedData.size() - 1) {
                keys += ", ";
                values += ", ";
            }
        }

        return update("insert into " + tableName + " ( " + keys + " ) values ( " + values + " )");
    }

	int64 ISqlClient::insertColumn(const String &tableName, const SqlFieldHead &columnHead) {
        return update("alter table " + tableName + " add " + organizeColumnInfo(columnHead));
    }

	int64 ISqlClient::insertColumn(const String &tableName, const SqlColumn &column) {
        for (uint32 i = 0; i < column.size(); ++i) {
            if (!insertColumn(tableName, *(column.getHead(i))))
                return -1;
        }
        return column.size() > 0;
    }

	int64 ISqlClient::deleteRow(const String &tableName, const SqlClause *where) {
        if (where != nullptr && where->type != SqlClauseType::Where)
            return -1;
        return update("delete from " + tableName + ' ' + organizeSqlClause(where, 1));
    }

	int64 ISqlClient::deleteColumn(const String &tableName, const String &columnName) {
        return update("alter table " + tableName + " drop column " + columnName);
    }

	int64 ISqlClient::createDatabase(const String &dbName) {
        return update("create database "+dbName);
    }

	int64 ISqlClient::deleteDatabase(const String &dbName) {
        return update("drop database "+dbName);
    }

	int64 ISqlClient::createTable(const String &tableName, const SqlColumn&column, const SqlTableInfo*tableInfo) {
        if(column.size()<=0)
            return -1;
        String sql = "create table " + tableName+" ( ";
        for(uint32 i=0;i<column.size();++i){
            sql += organizeColumnInfo(*(column[i].getHead()));
            if(i< column.size()-1)
                sql+=" , ";
        }
        return update(sql + " )");
    }

	int64 ISqlClient::deleteTable(const String &tableName) {
        return update("drop table "+tableName);
    }

    String ISqlClient::organizeColumnInfo(const SqlFieldHead &column) {
        // TODO: 应当扩展支持的更多属性
        return column.columnName + ' ' + SqlStructHelper::getDataTypeName(column.type) + (column.allowNull ? "" : " not null");
    }

    String ISqlClient::organizeSqlClause(const SqlClause *clauses, int clausesNum) {
        // TODO: 完善clause的结构然后补全此处
        String sql = "";

        return sql;
    }
	SqlTable ISqlClient::createSqlTable(const SqlFieldHead * heads, uint32 width, uint32 height){
		return SqlTable(heads, width, height);
	}
}