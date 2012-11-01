package com.virtual.waffledb.memory;

import com.virtual.waffledb.Column;
import com.virtual.waffledb.ColumnType;
import com.virtual.waffledb.ComparisonType;
import com.virtual.waffledb.Condition;
import com.virtual.waffledb.Database;
import com.virtual.waffledb.DatabaseException;
import com.virtual.waffledb.DatabaseRuntimeException;
import com.virtual.waffledb.Expression;
import com.virtual.waffledb.IntegerValue;
import com.virtual.waffledb.LiteralComparison;
import com.virtual.waffledb.SelectQueryBuilder;
import com.virtual.waffledb.ResultSet;
import com.virtual.waffledb.Selector;
import com.virtual.waffledb.StringValue;
import com.virtual.waffledb.TableDefinition;
import com.virtual.waffledb.Value;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import sun.reflect.generics.reflectiveObjects.NotImplementedException;

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

        final Index[] indicesByColumn = new Index[definition.columns.size()];
        for (final Map.Entry<String, Column> columnEntry : definition.columns.entrySet()) {
            final Column column = columnEntry.getValue();
            if (column.isIndexed) {
                indicesByColumn[column.index] = (column.isUnique
                        ? new UniqueHashIndex()
                        : new SortedIndex());
            }
        }

        tables.put(name, new Table(definition, indicesByColumn));
    }

    public void destroyTable(String name) throws DatabaseException {
        if (tables.remove(name) == null) {
            throw new DatabaseException("Inexistent table '" + name + "' cannot be removed");
        }
    }

    public SelectQueryBuilder createQueryBuilder() {
        return new MemorySelectQueryBuilder();
    }

    public ResultSet select(String tableName, SelectQueryBuilder query) throws DatabaseException {
        if (!(query instanceof MemorySelectQueryBuilder)) {
            throw new DatabaseException("Memory query expected");
        }

        final MemorySelectQueryBuilder memoryQuery = (MemorySelectQueryBuilder) query;
        final Table sourceTable = tables.get(tableName);
        final ArrayList<Value> resultElements = new ArrayList<Value>();

        try {
            final Iterator<Integer> affectedRows = getRowIndicesBySelector(sourceTable, new Selector(memoryQuery.conditions.toArray(new Condition[memoryQuery.conditions.size()])));

            while (affectedRows.hasNext()) {
                final int index = affectedRows.next();

                for (final MemoryExpression resultColumn : memoryQuery.resultColumns) {
                    resultElements.add(resultColumn.evaluate(sourceTable, index * sourceTable.definition.columns.size()));
                }
            }
        } catch (DatabaseRuntimeException ex) {
            ex.rethrow();
        }

        return new ResultSet(resultElements.toArray(new Value[resultElements.size()]), memoryQuery.resultColumns.size());
    }

    public void insert(String tableName, Value[] rows) throws DatabaseException {
        try {
            final Table table = tables.get(tableName);
            final TableDefinition tableDefinition = table.definition;
            if (rows.length % tableDefinition.columns.size() != 0) {
                throw new DatabaseException("Cannot insert incomplete row");
            }
            table.insertRows(rows);
        } catch (DatabaseRuntimeException ex) {
            ex.rethrow();
        }
    }

    public int delete(String tableName, Selector conditions) throws DatabaseException {
        final Table sourceTable = tables.get(tableName);
        int deletedRowCount = 0;

        try {
            final Iterator<Integer> affectedRows = getRowIndicesBySelector(
                    sourceTable,
                    conditions);

            while (affectedRows.hasNext()) {
                final int index = affectedRows.next();
                sourceTable.deleteRow(index);
                ++deletedRowCount;
            }
        } catch (DatabaseRuntimeException ex) {
            ex.rethrow();
        }

        return deletedRowCount;
    }

    public ColumnType getIntegerType() {
        return new IntegerType();
    }

    public ColumnType getStringType() {
        return new StringType();
    }

    private static Iterator<Integer> getRowIndicesBySelector(
            final Table table, final Selector selector) throws DatabaseException {
        final int size = table.getRowCount();

        return new Iterator<Integer>() {
            int current = -1;

            public boolean hasNext() {
                for (;;) {
                    current = table.getNextUsedRowIndex(1 + current);

                    assert (current >= 0);
                    assert (current <= size);

                    if (current >= size) {
                        return false;
                    }

                    if (selector == null) {
                        return true;
                    }

                    boolean fulfillsConditions = true;

                    for (final Condition condition : selector.conditions) {
                        final Value conditionValue = evaluateCondition(condition, table, current);
                        if (!(conditionValue instanceof IntegerValue)
                                || ((IntegerValue) conditionValue).value == 0) {
                            fulfillsConditions = false;
                            break;
                        }
                    }

                    if (fulfillsConditions) {
                        return true;
                    }
                }
            }

            public Integer next() {
                return current;
            }

            public void remove() {
                throw new UnsupportedOperationException("Not supported yet.");
            }

            private Value evaluateCondition(Condition condition, Table table, int currentRow) {
                final LiteralComparison literalCon = (LiteralComparison) condition;
                final IntegerValue literal = (IntegerValue) literalCon.literal;
                final IntegerValue element = (IntegerValue) table.getElement(currentRow * table.definition.columns.size() + literalCon.columnIndex);

                final long left = element.value;
                final long right = literal.value;
                boolean result;
                switch (literalCon.type) {
                case Equal:
                    result = (left == right);
                    break;
                case NotEqual:
                    result = (left != right);
                    break;
                case Less:
                    result = (left < right);
                    break;
                case LessEqual:
                    result = (left <= right);
                    break;
                case Greater:
                    result = (left > right);
                    break;
                case GreaterEqual:
                    result = (left >= right);
                    break;
                default:
                    throw new NotImplementedException();
                }
                return (result ? IntegerValue.True : IntegerValue.False);
            }
        };
    }

    public Expression createLiteral(final long value) {
        return new MemoryExpression() {
            public Value evaluate(Table source, int currentElement) throws DatabaseRuntimeException {
                return new IntegerValue(value);
            }

            public Iterator<Integer> preselectRows(Table source) throws DatabaseRuntimeException {
                throw new UnsupportedOperationException("Not supported yet.");
            }
        };
    }

    public Expression createLiteral(final String value) {
        return new MemoryExpression() {
            public Value evaluate(Table source, int currentElement) throws DatabaseRuntimeException {
                return new StringValue(value);
            }

            public Iterator<Integer> preselectRows(Table source) throws DatabaseRuntimeException {
                throw new UnsupportedOperationException("Not supported yet.");
            }
        };
    }

    public Expression createComparison(final ComparisonType comparison, final Expression left, final Expression right) {
        return new Comparison(comparison, (MemoryExpression) left, (MemoryExpression) right);
    }

    public Expression createColumnExpression(final String name, TableDefinition table) {
        final Column column = table.columns.get(name);
        final int elementOffset = column.index;
        return new ColumnExpression(elementOffset);
    }
}
