package com.virtual.waffledb.memory;

import com.virtual.waffledb.ColumnType;
import com.virtual.waffledb.StringValue;
import com.virtual.waffledb.Value;

/**
 *
 * @author virtual
 */
class StringType implements ColumnType {

    public String getName() {
        return "string";
    }

    public boolean isPossibleValue(Value value) {
        return (value instanceof StringValue);
    }
}
