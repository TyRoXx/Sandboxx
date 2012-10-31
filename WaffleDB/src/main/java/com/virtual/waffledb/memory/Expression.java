package com.virtual.waffledb.memory;

import com.virtual.waffledb.DatabaseException;
import com.virtual.waffledb.Value;

/**
 *
 * @author virtual
 */
interface Expression {

    Value evaluate(Table source, int currentElement) throws DatabaseException;
}
