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

    ResultSet select(String tableName, SelectQueryBuilder query) throws DatabaseException;

    void insert(String tableName, Value[] rows) throws DatabaseException;

    int delete(String tableName, Selector conditions) throws DatabaseException;

    ColumnType getIntegerType();

    ColumnType getStringType();

    Expression createLiteral(long value);

    Expression createLiteral(String value);

    Expression createComparison(ComparisonType comparison, Expression left, Expression right);

    Expression createColumnExpression(String name, TableDefinition table);
}
