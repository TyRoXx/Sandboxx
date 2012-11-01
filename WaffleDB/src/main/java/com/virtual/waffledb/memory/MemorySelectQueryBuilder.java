package com.virtual.waffledb.memory;

import com.virtual.waffledb.Column;
import com.virtual.waffledb.ComparisonType;
import com.virtual.waffledb.Condition;
import com.virtual.waffledb.DatabaseException;
import com.virtual.waffledb.DatabaseRuntimeException;
import com.virtual.waffledb.Expression;
import com.virtual.waffledb.IntegerValue;
import com.virtual.waffledb.SelectQueryBuilder;
import com.virtual.waffledb.StringValue;
import com.virtual.waffledb.TableDefinition;
import com.virtual.waffledb.Value;
import java.util.ArrayList;
import java.util.List;
import java.util.Stack;
import sun.reflect.generics.reflectiveObjects.NotImplementedException;

/**
 *
 * @author virtual
 */
class MemorySelectQueryBuilder implements SelectQueryBuilder {

    public TableDefinition sourceTable;
    public final List<MemoryExpression> resultColumns = new ArrayList<MemoryExpression>();
    public final ArrayList<Condition> conditions = new ArrayList<Condition>();
    private final Stack<MemoryExpression> expressionStack = new Stack<MemoryExpression>();

    public void setSourceTable(TableDefinition table) {
        this.sourceTable = table;
    }

    public void popResultColumn() {
        resultColumns.add(expressionStack.pop());
    }

    public void addCondition(Condition condition) {
        conditions.add(condition);
    }

    public void push(Expression expression) {
        expressionStack.push((MemoryExpression) expression);
    }
}
