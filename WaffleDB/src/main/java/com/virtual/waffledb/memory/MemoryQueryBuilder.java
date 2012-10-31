package com.virtual.waffledb.memory;

import com.virtual.waffledb.Column;
import com.virtual.waffledb.ComparisonType;
import com.virtual.waffledb.QueryBuilder;
import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

/**
 *
 * @author virtual
 */
public class MemoryQueryBuilder implements QueryBuilder {
    
    public String sourceTableName;
    public List<Expression> resultColumns = new ArrayList<Expression>();
    public Expression condition;
    private Stack<Expression> expressionStack = new Stack<Expression>();
    
    public void setSourceTable(String name) {
        throw new UnsupportedOperationException("Not supported yet.");
    }
    
    public void popResultColumn() {
        resultColumns.add(expressionStack.pop());
    }
    
    public void popCondition() {
        condition = expressionStack.pop();
    }
    
    public void pushColumn(String columnName) {
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
