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
    public final FastSearch[] fastSearchByColumn;

    public Table(TableDefinition definition, FastSearch[] fastSearchByColumn) {
        this.definition = definition;
        this.fastSearchByColumn = fastSearchByColumn;
        
        assert(definition.columns.size() == fastSearchByColumn.length);
    }
}
