package com.virtual.waffledb.memory;

import com.virtual.waffledb.ColumnType;
import com.virtual.waffledb.DatabaseException;
import com.virtual.waffledb.TableDefinition;
import com.virtual.waffledb.Value;
import java.util.ArrayList;
import java.util.BitSet;

/**
 *
 * @author virtual
 */
class Table {

    public final TableDefinition definition;
    private final ArrayList<Value> elements = new ArrayList<Value>();
    private final BitSet rowsInUse = new BitSet();
    private final Index[] indicesByColumn;

    public Table(TableDefinition definition, Index[] indicesByColumn) {
        this.definition = definition;
        this.indicesByColumn = indicesByColumn;

        assert (definition.columns.size() == indicesByColumn.length);
    }

    public int getElementCount() {
        return elements.size();
    }

    public Value getElement(int index) {
        return elements.get(index);
    }

    public int getRowCount() {
        return (elements.size() / definition.columns.size());
    }

    public void insertRows(Value[] inserted) throws DatabaseException {
        //TODO: make exception safe
        
        final int rowWidth = definition.columns.size();
        assert (inserted.length % rowWidth == 0);

        int endOfUsedRange = 0;
        int e = 0;
        while (e != inserted.length) {
            endOfUsedRange = rowsInUse.nextClearBit(endOfUsedRange);
            if (endOfUsedRange < 0
                    || (endOfUsedRange >= getRowCount())) {
                for (; e != inserted.length; ++e) {
                    final Value value = inserted[e];
                    checkColumnType(e, value);
                    elements.add(value);
                    rowsInUse.set(e % rowWidth, true);
                }
            } else {
                for (int i = 0; i < rowWidth; ++i, ++e) {
                    final Value value = inserted[e];
                    checkColumnType(e, value);
                    elements.set(endOfUsedRange * rowWidth + i, value);
                }
                rowsInUse.set(endOfUsedRange, true);
                ++endOfUsedRange;
            }
        }
    }

    private void checkColumnType(int columnOffset, Value value) throws DatabaseException {
        final int columnIndex = columnOffset % definition.columnsByIndex.length;
        final ColumnType requiredType = definition.columnsByIndex[columnIndex].type;
        if (!requiredType.isPossibleValue(value)) {
            throw new DatabaseException("Invalid value for column " + columnIndex);
        }
    }

    public void deleteRow(int rowIndex) {
        rowsInUse.set(rowIndex, false);

        assert (getNextUsedRowIndex(rowIndex) != rowIndex);
    }

    public int getNextUsedRowIndex(int rowIndex) {
        assert (rowIndex >= 0);
        int nextIndex = rowsInUse.nextSetBit(rowIndex);
        if (nextIndex < 0) {
            nextIndex = getRowCount();
        }
        assert (nextIndex >= rowIndex);
        return nextIndex;
    }

    public Index getIndexByColumn(int column) {
        return indicesByColumn[column];
    }
}
