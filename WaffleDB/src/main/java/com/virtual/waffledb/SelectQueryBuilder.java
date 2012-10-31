package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public interface SelectQueryBuilder {
    void setSourceTable(TableDefinition table);
    void popResultColumn();
    void popCondition();
    void push(Expression expression);
}
