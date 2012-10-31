package com.virtual.waffledb.memory;

import com.virtual.waffledb.ComparisonType;
import com.virtual.waffledb.Value;
import java.util.HashMap;
import java.util.Iterator;

/**
 *
 * @author virtual
 */
public class UniqueHashIndex implements Index {

    private final HashMap<Value, Integer> entries = new HashMap<Value, Integer>();

    public void add(Value key, int index) {
        final Integer previous = entries.put(key, index);
        assert (previous == null);
    }

    public void remove(Value key, int index) {
        final Integer previous = entries.remove(key);
        assert (previous == index);
    }

    public boolean isSupportedComparison(ComparisonType comparison) {
        return (comparison == ComparisonType.Equal);
    }

    public Iterator<Integer> find(Value key, ComparisonType comparison) {
        assert (isSupportedComparison(comparison));

        final Integer index = entries.get(key);

        return new Iterator<Integer>() {
            private Integer currentIndex = index;

            public boolean hasNext() {
                return (currentIndex != null);
            }

            public Integer next() {
                final Integer result = currentIndex;
                currentIndex = null;
                return result;
            }

            public void remove() {
                throw new UnsupportedOperationException("Not supported yet.");
            }
        };
    }
}
