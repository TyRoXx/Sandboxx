package com.virtual.waffledb.memory;

import com.virtual.waffledb.ColumnType;
import com.virtual.waffledb.IntegerValue;
import com.virtual.waffledb.Value;

/**
 *
 * @author virtual
 */
class IntegerType implements ColumnType {

    public String getName() {
        return "int";
    }

    public boolean isPossibleValue(Value value) {
        return (value instanceof IntegerValue);
    }
}
