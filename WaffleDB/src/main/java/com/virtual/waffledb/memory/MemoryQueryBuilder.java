package com.virtual.waffledb.memory;

import com.virtual.waffledb.Column;
import com.virtual.waffledb.ComparisonType;
import com.virtual.waffledb.QueryBuilder;
import java.util.ArrayList;
import java.util.Stack;

/**
 *
 * @author virtual
 */
public class MemoryQueryBuilder implements QueryBuilder {
    public String sourceTableName;
    public ArrayList<String> resultColumns;
    public Expression condition;
    private Stack<Expression> expressionStack = new Stack<Expression>();
    
    public void setSourceTable(String name) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void addResultColumn(String columnName) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void popCondition() {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void pushColumn(Column column) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void pushInteger(long value) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void pushString(String value) {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void pushComparison(ComparisonType type) {
        throw new UnsupportedOperationException("Not supported yet.");
    }
    
}
