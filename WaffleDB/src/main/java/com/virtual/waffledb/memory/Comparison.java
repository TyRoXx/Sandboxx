package com.virtual.waffledb.memory;

import com.virtual.waffledb.ComparisonType;
import com.virtual.waffledb.DatabaseRuntimeException;
import com.virtual.waffledb.Expression;
import com.virtual.waffledb.IntegerValue;
import com.virtual.waffledb.Value;
import java.util.Iterator;
import sun.reflect.generics.reflectiveObjects.NotImplementedException;

/**
 *
 * @author virtual
 */
class Comparison implements MemoryExpression {

    private final ComparisonType type;
    private final MemoryExpression left, right;

    public Comparison(ComparisonType type, MemoryExpression left, MemoryExpression right) {
        this.type = type;
        this.left = left;
        this.right = right;
    }

    public Value evaluate(Table source, int currentElement) throws DatabaseRuntimeException {
        final Value leftValue = ((MemoryExpression) left).evaluate(source, currentElement);
        final Value rightValue = ((MemoryExpression) right).evaluate(source, currentElement);
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

    public Iterator<Integer> preselectRows(Table source) throws DatabaseRuntimeException {
        ColumnExpression columnExpression = null;
        if (left instanceof ColumnExpression) {
            columnExpression = (ColumnExpression) left;
        }
        if (right instanceof ColumnExpression) {
            columnExpression = (ColumnExpression) right;
        }
        return null; //TODO
    }
}
