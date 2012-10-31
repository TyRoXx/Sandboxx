package com.virtual.waffledb.memory;

import com.virtual.waffledb.DatabaseException;

/**
 *
 * @author virtual
 */
public interface Expression {

    void evaluate(Table source) throws DatabaseException;
}
