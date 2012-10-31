package com.virtual.waffledb.memory;

import com.virtual.waffledb.ColumnType;
import com.virtual.waffledb.Database;
import com.virtual.waffledb.DatabaseException;
import com.virtual.waffledb.QueryBuilder;
import com.virtual.waffledb.TableDefinition;
import java.util.Map;

/**
 *
 * @author virtual
 */
public class MemoryDatabase implements Database {

    public Map<String, TableDefinition> getTables() throws DatabaseException {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void createTable(String name, TableDefinition definition) throws DatabaseException {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public void destroyTable(String name) throws DatabaseException {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public QueryBuilder createQueryBuilder() {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public ColumnType getIntegerType() {
        throw new UnsupportedOperationException("Not supported yet.");
    }

    public ColumnType getStringType() {
        throw new UnsupportedOperationException("Not supported yet.");
    }
}
