package com.virtual.waffledb.memory;

import com.virtual.waffledb.Column;
import com.virtual.waffledb.ColumnType;
import com.virtual.waffledb.ComparisonType;
import com.virtual.waffledb.Database;
import com.virtual.waffledb.DatabaseException;
import com.virtual.waffledb.DatabaseRuntimeException;
import com.virtual.waffledb.Expression;
import com.virtual.waffledb.IntegerValue;
import com.virtual.waffledb.SelectQueryBuilder;
import com.virtual.waffledb.ResultSet;
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

        final FastSearch[] fastSearchByColumn = new FastSearch[definition.columns.size()];
        for (final Map.Entry<String, Column> columnEntry : definition.columns.entrySet()) {
            final Column column = columnEntry.getValue();
            if (column.hasFastSearch) {
                if (column.isUnique) {
                    fastSearchByColumn[column.index] = new UniqueHashSearch();
                }
            }
        }

        tables.put(name, new Table(definition, fastSearchByColumn));
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
        final TableDefinition sourceTableDefinition = sourceTable.definition;
        final ArrayList<Value> resultElements = new ArrayList<Value>();

        try {
            final Iterator<Integer> affectedRows = getRowIndicesByCondition(sourceTable, memoryQuery.condition);

            while (affectedRows.hasNext()) {
                final int index = affectedRows.next();

                for (final MemoryExpression resultColumn : memoryQuery.resultColumns) {
                    resultElements.add(resultColumn.evaluate(sourceTable, index));
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

    public int delete(String tableName, Expression condition) throws DatabaseException {
        final Table sourceTable = tables.get(tableName);
        int deletedRowCount = 0;

        try {
            final Iterator<Integer> affectedRows = getRowIndicesByCondition(sourceTable, (MemoryExpression) condition);

            while (affectedRows.hasNext()) {
                final int index = affectedRows.next();
                sourceTable.deleteRow(index);
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

    private static Iterator<Integer> getRowIndicesByCondition(
            final Table table, final MemoryExpression condition) throws DatabaseException {
        final int size = table.getElementCount();

        return new Iterator<Integer>() {
            int current = 0;

            public boolean hasNext() {
                for (;;) {
                    assert (current % table.definition.columns.size() == 0);

                    if (current == size) {
                        return false;
                    }

                    if (condition != null) {
                        final Value conditionValue = condition.evaluate(table, current);
                        if ((conditionValue instanceof IntegerValue)
                                && ((IntegerValue) conditionValue).value != 0) {
                            return true;
                        } else {
                            next();
                            continue;
                        }
                    }

                    return true;
                }
            }

            public Integer next() {
                final int result = current;
                current = table.getNextUsedRowIndex(1 + current / table.definition.columns.size()) * table.definition.columns.size();
                return result;
            }

            public void remove() {
                throw new UnsupportedOperationException("Not supported yet.");
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
        return new MemoryExpression() {
            public Value evaluate(Table source, int currentElement) throws DatabaseRuntimeException {
                final Value leftValue = ((MemoryExpression) left).evaluate(source, currentElement);
                final Value rightValue = ((MemoryExpression) right).evaluate(source, currentElement);
                if (leftValue instanceof IntegerValue
                        && rightValue instanceof IntegerValue) {
                    final long leftInteger = ((IntegerValue) leftValue).value;
                    final long rightInteger = ((IntegerValue) rightValue).value;
                    boolean result;
                    switch (comparison) {
                        case Equal:
                            result = (leftInteger == rightInteger);
                            break;
                        case NotEqual:
                            result = (leftInteger != rightInteger);
                            break;
                        case Less:
                            result = (leftInteger < rightInteger);
                            break;
                        case LessEqual:
                            result = (leftInteger <= rightInteger);
                            break;
                        case Greater:
                            result = (leftInteger > rightInteger);
                            break;
                        case GreaterEqual:
                            result = (leftInteger >= rightInteger);
                            break;
                        default:
                            throw new NotImplementedException();
                    }
                    return (result ? IntegerValue.True : IntegerValue.False);
                } else {
                    throw new DatabaseRuntimeException("Only values of the same type can be compared");
                }
            }

            public Iterator<Integer> preselectRows(Table source) throws DatabaseRuntimeException {
                ColumnExpression columnExpression = null;
                if (left instanceof ColumnExpression) {
                    columnExpression = (ColumnExpression) left;
                }
                if (right instanceof ColumnExpression) {
                    columnExpression = (ColumnExpression) right;
                }
                return null; //TODO
            }
        };
    }

    public Expression createColumnExpression(final String name, TableDefinition table) {
        final Column column = table.columns.get(name);
        final int elementOffset = column.index;
        return new ColumnExpression(elementOffset);
    }
}
