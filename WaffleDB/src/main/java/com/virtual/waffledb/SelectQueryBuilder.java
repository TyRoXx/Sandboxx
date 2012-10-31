package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public interface SelectQueryBuilder {
    void setSourceTable(TableDefinition table);
    void popResultColumn();
    void popCondition();
    void pushColumn(String columnName);
    void pushInteger(long value);
    void pushString(String value);
    void pushComparison(ComparisonType type);
}
