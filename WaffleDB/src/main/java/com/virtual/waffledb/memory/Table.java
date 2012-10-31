package com.virtual.waffledb.memory;

import com.virtual.waffledb.TableDefinition;
import com.virtual.waffledb.Value;
import java.util.ArrayList;

/**
 *
 * @author virtual
 */
class Table {

    public final TableDefinition definition;
    public final ArrayList<Value> elements = new ArrayList<Value>();

    public Table(TableDefinition definition) {
        this.definition = definition;
    }
}
