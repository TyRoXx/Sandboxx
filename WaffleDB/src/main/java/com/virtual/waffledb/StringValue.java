package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public class StringValue implements Value {
    public final String value;
    
    public StringValue(String value) {
        this.value = value;
    }

    public void accept(ValueVisitor visitor) {
        visitor.visit(this);
    }
}
