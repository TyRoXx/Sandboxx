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

            final SelectQueryBuilder query = db.createQueryBuilder();
            query.setSourceTable(db.getTables().get("lang"));
            query.pushColumn("name");
            query.popResultColumn();
            query.pushColumn("number");
            query.popResultColumn();
            System.out.println("Query created");
            
            final ResultSet results = db.executeQuery("lang", query);
            System.out.println("Query executed");
            System.out.println(results.getRowCount() + " rows");
            
            for (int i = 0; i < results.elements.length; i += results.rowLength)
            {
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
