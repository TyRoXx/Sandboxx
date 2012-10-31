package com.virtual.waffledb;

import java.util.Map;

/**
 *
 * @author virtual
 */
public interface Database {

    Map<String, TableDefinition> getTables() throws DatabaseException;

    void createTable(String name, TableDefinition definition) throws DatabaseException;

    void destroyTable(String name) throws DatabaseException;

    SelectQueryBuilder createQueryBuilder();

    ResultSet executeQuery(String tableName, SelectQueryBuilder query) throws DatabaseException;

    ColumnType getIntegerType();

    ColumnType getStringType();
}
