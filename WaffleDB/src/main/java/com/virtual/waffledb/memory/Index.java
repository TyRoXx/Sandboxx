package com.virtual.waffledb.memory;

import com.virtual.waffledb.ComparisonType;
import com.virtual.waffledb.Value;
import java.util.Iterator;

/**
 *
 * @author virtual
 */
public interface Index {
    void add(Value key, int index);
    void remove(Value key, int index);
    boolean isSupportedComparison(ComparisonType comparison);
    Iterator<Integer> find(Value key, ComparisonType comparison);
}
