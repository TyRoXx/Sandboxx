package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public class Column {

    public final ColumnType type;
    public final int index;

    public Column(ColumnType type, int index) {
        this.type = type;
        this.index = index;
    }
}
