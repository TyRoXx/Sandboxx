package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public class Column {

    public final ColumnType type;
    public final boolean isUnique;
    public final boolean hasFastSearch;
    public final int index;

    public Column(ColumnType type, boolean isUnique, boolean hasFastSearch, int index) {
        this.type = type;
        this.isUnique = isUnique;
        this.hasFastSearch = hasFastSearch;
        this.index = index;
    }
}
