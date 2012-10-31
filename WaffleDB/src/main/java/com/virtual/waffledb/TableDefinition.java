package com.virtual.waffledb;

import java.util.Map;

/**
 *
 * @author virtual
 */
public class TableDefinition {

    public final Map<String, Column> columns;

    public TableDefinition(Map<String, Column> columns) {
        this.columns = columns;
    }
}
