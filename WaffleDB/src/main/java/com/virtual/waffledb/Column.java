package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public class Column {

    public final String name;
    public final ColumnType type;

    public Column(String name, ColumnType type) {
        this.name = name;
        this.type = type;
    }
}
