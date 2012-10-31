package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public interface ValueVisitor {
    void visit(IntegerValue integer);
    void visit(StringValue string);
}
