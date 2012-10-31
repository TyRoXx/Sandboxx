package com.virtual.waffledb.memory;

import com.virtual.waffledb.DatabaseException;
import com.virtual.waffledb.DatabaseRuntimeException;
import com.virtual.waffledb.Expression;
import com.virtual.waffledb.Value;
import java.util.Iterator;

/**
 *
 * @author virtual
 */
interface MemoryExpression extends Expression {

    Iterator<Integer> preselectRows(Table source) throws DatabaseRuntimeException;
    Value evaluate(Table source, int currentElement) throws DatabaseRuntimeException;
}
