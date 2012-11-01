package com.virtual.waffledb.memory;

import com.virtual.waffledb.Condition;
import com.virtual.waffledb.Expression;
import com.virtual.waffledb.SelectQueryBuilder;
import com.virtual.waffledb.TableDefinition;
import java.util.ArrayList;
import java.util.List;

/**
 *
 * @author virtual
 */
class MemorySelectQueryBuilder implements SelectQueryBuilder {

    public TableDefinition sourceTable;
    public final List<MemoryExpression> resultColumns = new ArrayList<MemoryExpression>();
    public final ArrayList<Condition> conditions = new ArrayList<Condition>();

    public void setSourceTable(TableDefinition table) {
        this.sourceTable = table;
    }

    public void addCondition(Condition condition) {
        conditions.add(condition);
    }

    public void addResultColumn(Expression expression) {
        resultColumns.add((MemoryExpression) expression);
    }
}
