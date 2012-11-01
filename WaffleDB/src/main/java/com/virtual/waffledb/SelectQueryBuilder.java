package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public interface SelectQueryBuilder {
    void setSourceTable(TableDefinition table);
    void popResultColumn();
    void addCondition(Condition condition);
    void push(Expression expression);
}
