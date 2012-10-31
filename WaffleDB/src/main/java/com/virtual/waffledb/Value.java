package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public interface Value {

    void accept(ValueVisitor visitor);
}
