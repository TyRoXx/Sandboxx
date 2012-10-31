package com.virtual.waffledb.memory;

import com.virtual.waffledb.Column;
import com.virtual.waffledb.ComparisonType;
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
    public List<MemoryExpression> resultColumns = new ArrayList<MemoryExpression>();
    public MemoryExpression condition;
    private Stack<MemoryExpression> expressionStack = new Stack<MemoryExpression>();

    public void setSourceTable(TableDefinition table) {
        this.sourceTable = table;
    }

    public void popResultColumn() {
        resultColumns.add(expressionStack.pop());
    }

    public void popCondition() {
        condition = expressionStack.pop();
    }

    public void push(Expression expression) {
        expressionStack.push((MemoryExpression) expression);
    }
}
