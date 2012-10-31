package com.virtual.waffledb.memory;

import com.virtual.waffledb.ColumnType;
import com.virtual.waffledb.Database;
import com.virtual.waffledb.DatabaseException;
import com.virtual.waffledb.IntegerValue;
import com.virtual.waffledb.QueryBuilder;
import com.virtual.waffledb.ResultSet;
import com.virtual.waffledb.TableDefinition;
import com.virtual.waffledb.Value;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

/**
 *
 * @author virtual
 */
public class MemoryDatabase implements Database {

    private Map<String, Table> tables = new HashMap<String, Table>();

    public Map<String, TableDefinition> getTables() throws DatabaseException {
        final Map<String, TableDefinition> definitions = new HashMap<String, TableDefinition>();
        for (final Map.Entry<String, Table> t : tables.entrySet()) {
            definitions.put(t.getKey(), t.getValue().definition);
        }
        return definitions;
    }

    public void createTable(String name, TableDefinition definition) throws DatabaseException {
        if (tables.get(name) != null) {
            throw new DatabaseException("Table '" + name
                    + "' already exists");
        }
        tables.put(name, new Table(definition));
    }

    public void destroyTable(String name) throws DatabaseException {
        if (tables.remove(name) == null) {
            throw new DatabaseException("Inexistent table '" + name + "' cannot be removed");
        }
    }

    public QueryBuilder createQueryBuilder() {
        return new MemoryQueryBuilder();
    }

    public ResultSet executeQuery(String tableName, QueryBuilder query) throws DatabaseException {
        if (!(query instanceof MemoryQueryBuilder)) {
            throw new DatabaseException("Memory query expected");
        }

        final MemoryQueryBuilder memoryQuery = (MemoryQueryBuilder) query;
        final Table sourceTable = tables.get(tableName);
        final TableDefinition sourceTableDefinition = sourceTable.definition;
        final ArrayList<Value> resultElements = new ArrayList<Value>();

        for (int i = 0; i < sourceTable.elements.size(); i += sourceTableDefinition.columns.size()) {
            final Value condition = memoryQuery.condition.evaluate(sourceTable, i);
            if ((condition instanceof IntegerValue)
                    && ((IntegerValue) condition).value != 0) {
                for (final Expression resultColumn : memoryQuery.resultColumns) {
                    resultElements.add(resultColumn.evaluate(sourceTable, i));
                }
            }
        }

        return new ResultSet(resultElements.toArray(new Value[resultElements.size()]), memoryQuery.resultColumns.size());
    }

    public ColumnType getIntegerType() {
        return new IntegerType();
    }

    public ColumnType getStringType() {
        return new StringType();
    }
}
