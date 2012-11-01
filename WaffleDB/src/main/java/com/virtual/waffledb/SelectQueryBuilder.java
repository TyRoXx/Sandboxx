package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public interface SelectQueryBuilder {
    void setSourceTable(TableDefinition table);
    void addCondition(Condition condition);
    void addResultColumn(Expression expression);
}
