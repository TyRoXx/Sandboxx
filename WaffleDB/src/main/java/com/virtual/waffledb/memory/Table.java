package com.virtual.waffledb.memory;

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
    private final FastSearch[] fastSearchByColumn;

    public Table(TableDefinition definition, FastSearch[] fastSearchByColumn) {
        this.definition = definition;
        this.fastSearchByColumn = fastSearchByColumn;

        assert (definition.columns.size() == fastSearchByColumn.length);
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

    public void insertRows(Value[] inserted) {
        final int rowWidth = definition.columns.size();
        assert (inserted.length % rowWidth == 0);

        int endOfUsedRange = 0;
        int e = 0;
        while (e != inserted.length) {
            endOfUsedRange = rowsInUse.nextClearBit(endOfUsedRange);
            if (endOfUsedRange < 0
                    || (endOfUsedRange >= getRowCount())) {
                for (; e != inserted.length; ++e) {
                    elements.add(inserted[e]);
                    rowsInUse.set(e % rowWidth, true);
                }
            } else {
                for (int i = 0; i < rowWidth; ++i, ++e) {
                    elements.set(endOfUsedRange * rowWidth + i, inserted[e]);
                }
                rowsInUse.set(endOfUsedRange, true);
                ++endOfUsedRange;
            }
        }
    }

    public void deleteRow(int rowIndex) {
        rowsInUse.set(rowIndex, false);
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

    public FastSearch getFastSearchByColumn(int column) {
        return fastSearchByColumn[column];
    }
}
