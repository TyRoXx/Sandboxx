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
            columns.put("name", new Column(db.getStringType(), 0));
            columns.put("number", new Column(db.getIntegerType(), 1));
            db.createTable("lang", new TableDefinition(columns));
            System.out.println("Table created");

            db.insert("lang", new Value[]{
                        new StringValue("Java"), new IntegerValue(123),
                        new StringValue("C++"), new IntegerValue(456)
                    });
            db.insert("lang", new Value[]{});
            System.out.println("Values inserted");

            final SelectQueryBuilder select = db.createQueryBuilder();
            select.setSourceTable(db.getTables().get("lang"));
            select.pushColumn("name");
            select.popResultColumn();
            select.pushColumn("number");
            select.popResultColumn();
            System.out.println("Select query created");

            final ResultSet results = db.select("lang", select);
            System.out.println("Select query executed");
            System.out.println(results.getRowCount() + " rows");

            for (int i = 0; i < results.elements.length; i += results.rowLength) {
                System.out.print(results.elements[i]);
                System.out.print(", ");
                System.out.print(results.elements[i + 1]);
                System.out.println("");
            }
        } catch (final DatabaseException ex) {
            System.out.println(ex.getMessage());
        }
    }
}
