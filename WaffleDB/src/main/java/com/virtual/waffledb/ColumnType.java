package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public interface ColumnType {
    String getName();
    boolean isPossibleValue(Value value);
}
