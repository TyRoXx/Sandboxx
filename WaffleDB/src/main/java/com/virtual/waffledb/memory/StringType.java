package com.virtual.waffledb.memory;

import com.virtual.waffledb.ColumnType;

/**
 *
 * @author virtual
 */
public class StringType implements ColumnType {

    public String getName() {
        return "string";
    }
}
