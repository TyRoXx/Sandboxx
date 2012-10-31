package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public class Column {

    public final ColumnType type;
    public final boolean isUnique;
    public final boolean isIndexed;
    public final int index;

    public Column(ColumnType type, boolean isUnique, boolean isIndexed, int index) {
        this.type = type;
        this.isUnique = isUnique;
        this.isIndexed = isIndexed;
        this.index = index;
    }
}
