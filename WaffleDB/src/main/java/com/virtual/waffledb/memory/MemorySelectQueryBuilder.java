package com.virtual.waffledb.memory;

import com.virtual.waffledb.Column;
import com.virtual.waffledb.ComparisonType;
import com.virtual.waffledb.DatabaseException;
import com.virtual.waffledb.DatabaseRuntimeException;
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

    public void pushColumn(String columnName) {
        final Column column = sourceTable.columns.get(columnName);
        final int elementOffset = column.index;

        expressionStack.push(new MemoryExpression() {
            public Value evaluate(Table source, int currentElement) throws DatabaseRuntimeException {
                return source.elements.get(currentElement + elementOffset);
            }
        });
    }

    public void pushInteger(final long value) {
        expressionStack.push(new MemoryExpression() {
            public Value evaluate(Table source, int currentElement) throws DatabaseRuntimeException {
                return new IntegerValue(value);
            }
        });
    }

    public void pushString(final String value) {
        expressionStack.push(new MemoryExpression() {
            public Value evaluate(Table source, int currentElement) throws DatabaseRuntimeException {
                return new StringValue(value);
            }
        });
    }

    public void pushComparison(final ComparisonType type) {
        final MemoryExpression right = expressionStack.pop();
        final MemoryExpression left = expressionStack.pop();
        final MemoryExpression comparison = new MemoryExpression() {
            public Value evaluate(Table source, int currentElement) throws DatabaseRuntimeException {
                final Value leftValue = left.evaluate(source, currentElement);
                final Value rightValue = right.evaluate(source, currentElement);
                if (leftValue instanceof IntegerValue
                        && rightValue instanceof IntegerValue) {
                    final long leftInteger = ((IntegerValue) leftValue).value;
                    final long rightInteger = ((IntegerValue) rightValue).value;
                    boolean result;
                    switch (type) {
                        case Equal:
                            result = (leftInteger == rightInteger);
                            break;
                        case NotEqual:
                            result = (leftInteger != rightInteger);
                            break;
                        case Less:
                            result = (leftInteger < rightInteger);
                            break;
                        case LessEqual:
                            result = (leftInteger <= rightInteger);
                            break;
                        case Greater:
                            result = (leftInteger > rightInteger);
                            break;
                        case GreaterEqual:
                            result = (leftInteger >= rightInteger);
                            break;
                        default:
                            throw new NotImplementedException();
                    }
                    return (result ? IntegerValue.True : IntegerValue.False);
                } else {
                    throw new DatabaseRuntimeException("Only values of the same type can be compared");
                }
            }
        };
        expressionStack.push(comparison);
    }
}
