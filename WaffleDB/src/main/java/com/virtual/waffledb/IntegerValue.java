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
}
