package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public class IntegerValue implements Value {

    public final long value;
    public static final IntegerValue True = new IntegerValue(1);
    public static final IntegerValue False = new IntegerValue(0);

    public IntegerValue(long value) {
        this.value = value;
    }

    public void accept(ValueVisitor visitor) {
        visitor.visit(this);
    }

    @Override
    public String toString() {
        return "" + value;
    }

    @Override
    public int hashCode() {
        return (int) (value | (value >> 32));
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final IntegerValue other = (IntegerValue) obj;
        return this.value == other.value;
    }
}
