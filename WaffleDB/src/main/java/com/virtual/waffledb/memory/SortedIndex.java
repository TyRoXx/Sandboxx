package com.virtual.waffledb.memory;

import com.virtual.waffledb.ComparisonType;
import com.virtual.waffledb.Value;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;
import java.util.SortedMap;
import java.util.TreeMap;

/**
 *
 * @author virtual
 */
public class SortedIndex implements Index {

    private final SortedMap<Value, Set<Integer>> map = new TreeMap<Value, Set<Integer>>();

    public void add(Value key, int index) {
        final Set<Integer> indices = map.get(key);
        if (indices != null) {
            indices.add(index);
        } else {
            final Set<Integer> newIndices = new HashSet<Integer>();
            newIndices.add(index);
            map.put(key, newIndices);
        }
    }

    public void remove(Value key, int index) {
        final Set<Integer> indices = map.get(key);
        assert (indices != null);
        indices.remove(index);
    }

    public boolean isSupportedComparison(ComparisonType comparison) {
        switch (comparison) {
        case Equal:
        case NotEqual:
        case Less:
        case LessEqual:
        case Greater:
        case GreaterEqual:
            return true;

        default:
            return false;
        }
    }

    public Iterator<Integer> find(Value key, ComparisonType comparison) {
        throw new UnsupportedOperationException("Not supported yet.");
    }
}
