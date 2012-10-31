package com.virtual.waffledb.memory;

import com.virtual.waffledb.DatabaseRuntimeException;
import com.virtual.waffledb.Value;
import java.util.Iterator;

/**
 *
 * @author virtual
 */
class ColumnExpression implements MemoryExpression {

    private final int columnIndex;

    public ColumnExpression(int columnIndex) {
        this.columnIndex = columnIndex;
    }

    public Value evaluate(Table source, int currentElement) throws DatabaseRuntimeException {
        return source.getElement(currentElement + columnIndex);
    }

    public Iterator<Integer> preselectRows(Table source) throws DatabaseRuntimeException {
        return null;
    }
}
