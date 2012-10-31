package com.virtual.waffledb;

import com.virtual.waffledb.memory.MemoryDatabase;
import java.util.HashMap;
import java.util.Map;

/**
 * Hello world!
 *
 */
public class App {

    public static void main(String[] args) {
        final Database db = new MemoryDatabase();
        try {
            final Map<String, Column> columns = new HashMap<String, Column>();
            columns.put("name", new Column(db.getStringType(), true, false, 0));
            columns.put("comment", new Column(db.getStringType(), false, false, 1));
            columns.put("number", new Column(db.getIntegerType(), false, true, 2));
            columns.put("id", new Column(db.getIntegerType(), true, true, 3));
            db.createTable("lang", new TableDefinition(columns));
            System.out.println("Table created");

            db.insert("lang", new Value[]{
                        new StringValue("Java"), new StringValue(""), new IntegerValue(123), new IntegerValue(0),
                        new StringValue("C++"), new StringValue(""), new IntegerValue(456), new IntegerValue(1),
                        new StringValue("C"), new StringValue(""), new IntegerValue(789), new IntegerValue(2)
                    });
            db.insert("lang", new Value[]{});
            System.out.println("Values inserted");

            final SelectQueryBuilder select = db.createQueryBuilder();
            select.setSourceTable(db.getTables().get("lang"));
            select.pushColumn("id");
            select.popResultColumn();
            select.pushColumn("name");
            select.popResultColumn();
            select.pushColumn("number");
            select.popResultColumn();
            select.pushColumn("comment");
            select.popResultColumn();

            select.pushColumn("id");
            select.pushInteger(1);
            select.pushComparison(ComparisonType.NotEqual);
            select.popCondition();

            System.out.println("Select query created");

            final ResultSet results = db.select("lang", select);
            System.out.println("Select query executed");
            System.out.println(results.getRowCount() + " rows");

            for (int i = 0; i < results.elements.length; i += results.rowLength) {
                for (int j = 0; j < results.rowLength; ++j) {
                    System.out.print(results.elements[i + j] + ", ");
                }
                System.out.println("");
            }
        } catch (final DatabaseException ex) {
            System.out.println(ex.getMessage());
        }
    }
}
