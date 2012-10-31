package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public interface QueryBuilder {
    void setSourceTable(String name);
    void addResultColumn(String columnName);
    void popCondition();
    void pushColumn(Column column);
    void pushInteger(long value);
    void pushString(String value);
    void pushComparison(ComparisonType type);
}
