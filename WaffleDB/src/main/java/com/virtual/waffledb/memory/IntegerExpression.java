package com.virtual.waffledb.memory;

/**
 *
 * @author virtual
 */
public class IntegerExpression implements Expression {
    public final long value;
    
    public IntegerExpression(long value) {
        this.value = value;
    }
}
