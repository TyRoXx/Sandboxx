package com.virtual.waffledb;

import com.virtual.waffledb.memory.MemoryDatabase;
import java.util.Arrays;
import java.util.Map;
import java.util.TreeMap;
import junit.framework.Test;
import junit.framework.TestCase;
import junit.framework.TestSuite;

public class AppTest
        extends TestCase {

    /**
     * Create the test case
     *
     * @param testName name of the test case
     */
    public AppTest(String testName) {
        super(testName);
    }

    /**
     * @return the suite of tests being tested
     */
    public static Test suite() {
        return new TestSuite(AppTest.class);
    }

    public void testApp() {
        final Database db = new MemoryDatabase();
        final Map<String, Column> columns = new TreeMap<String, Column>();
        columns.put("id", new Column(db.getIntegerType(), true, true, 0));
        columns.put("name", new Column(db.getStringType(), false, false, 1));
        columns.put("count", new Column(db.getIntegerType(), false, true, 2));
        columns.put("otherid", new Column(db.getIntegerType(), true, false, 3));
        try {
            //make sure a table name can be reused safely
            for (int i = 0; i < 2; ++i) {
                final String table = "tbl";
                db.createTable(table, new TableDefinition(columns));
                testTable(db, table);
                db.destroyTable(table);
            }
        } catch (DatabaseException ex) {
            fail(ex.getMessage());
        }
    }

    private void testTable(Database db, String table) throws DatabaseException {
        final TableDefinition tableDef = db.getTables().get(table);
        assertFalse(tableDef == null);

        final Value[] values = new Value[]{
            new IntegerValue(0),
            new StringValue("a0"),
            new IntegerValue(12),
            new IntegerValue(-1),
            new IntegerValue(1),
            new StringValue("a1"),
            new IntegerValue(24),
            new IntegerValue(-2),
            new IntegerValue(2),
            new StringValue("a2"),
            new IntegerValue(36),
            new IntegerValue(-3),};

        {
            db.insert(table, values);

            //no conditions mean delete everything
            final int deleted = db.delete(table, null);
            assertEquals(3, deleted);
        }

        {
            db.insert(table, values);

            //delete by ids 0-15 and expect it to do something exactly for ids 0-2
            for (int i = 0; i < 16; ++i) {
                final int deleted = db.delete(table, new Selector(new Condition[]{
                            new LiteralComparison(0, ComparisonType.Equal, new IntegerValue(i))
                        }));
                assertEquals((i < 3) ? 1 : 0, deleted);
            }
        }

        {
            db.insert(table, values);

            final SelectQueryBuilder query = db.createQueryBuilder();
            query.addResultColumn(db.createColumnExpression("id", tableDef));

            {
                final ResultSet results = db.select(table, query);
                assertEquals(3, results.getRowCount());
                assertEquals(1, results.rowLength);
                assertTrue(Arrays.equals(new Value[]{
                            new IntegerValue(0), new IntegerValue(1), new IntegerValue(2)},
                        results.elements));
            }

            query.addCondition(new LiteralComparison(0, ComparisonType.NotEqual, new IntegerValue(1)));

            {
                final ResultSet results = db.select(table, query);
                assertEquals(2, results.getRowCount());
                assertEquals(1, results.rowLength);
                assertTrue(Arrays.equals(new Value[]{
                            new IntegerValue(0), new IntegerValue(2)},
                        results.elements));
            }

            query.addCondition(new LiteralComparison(0, ComparisonType.Equal, new IntegerValue(999)));

            {
                final ResultSet results = db.select(table, query);
                assertEquals(0, results.getRowCount());
                assertEquals(1, results.rowLength);
            }

            final int deleted = db.delete(table, null);
            assertEquals(3, deleted);
        }

        {
            final Value[] invalidValues = new Value[]
            {
                new IntegerValue(11),
                new IntegerValue(123), //String expected
                new StringValue("hallo"), //Integer expected
                new IntegerValue(0),
            };

            boolean caught = false;
            try
            {
                db.insert(table, invalidValues);
            }
            catch (DatabaseException ex) {
                caught = true;
            }

            assertTrue(caught);
        }
    }
}
