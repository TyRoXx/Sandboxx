package com.virtual.waffledb.memory;

import com.virtual.waffledb.DatabaseException;
import com.virtual.waffledb.DatabaseRuntimeException;
import com.virtual.waffledb.Expression;
import com.virtual.waffledb.Value;

/**
 *
 * @author virtual
 */
interface MemoryExpression extends Expression {

    Value evaluate(Table source, int currentElement) throws DatabaseRuntimeException;
}
