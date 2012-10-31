package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public interface QueryBuilder {
    void setSourceTable(String name);
    void popResultColumn();
    void popCondition();
    void pushColumn(String columnName);
    void pushInteger(long value);
    void pushString(String value);
    void pushComparison(ComparisonType type);
}
