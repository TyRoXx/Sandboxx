package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public class ResultSet {

    public final Value[] elements;
    public final int rowLength;

    public ResultSet(Value[] elements, int rowLength) {
        this.elements = elements;
        this.rowLength = rowLength;
        assert (this.elements.length % rowLength == 0);
    }

    public int getRowCount() {
        return (elements.length / rowLength);
    }
}
