package com.virtual.waffledb;

import java.util.Map;

/**
 *
 * @author virtual
 */
public class TableDefinition {

    public final Map<String, Column> columns;
    public final Column[] columnsByIndex;

    public TableDefinition(Map<String, Column> columns) {
        this.columns = columns;
        this.columnsByIndex = new Column[columns.size()];

        for (final Map.Entry<String, Column> columnEntry : columns.entrySet()) {
            final Column column = columnEntry.getValue();
            final int index = column.index;
            assert (columnsByIndex[index] == null);
            columnsByIndex[index] = column;
        }
    }
}
