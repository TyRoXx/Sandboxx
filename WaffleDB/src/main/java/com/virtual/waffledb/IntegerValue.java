package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public class IntegerValue implements Value {
    public final long value;
    
    public IntegerValue(long value) {
        this.value = value;
    }

    public void accept(ValueVisitor visitor) {
        visitor.visit(this);
    }
}
