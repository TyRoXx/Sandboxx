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
            final TableDefinition langTable = new TableDefinition(columns);
            db.createTable("lang", langTable);
            System.out.println("Table created");

            db.insert("lang", new Value[]{
                        new StringValue("Java"), new StringValue(""), new IntegerValue(123), new IntegerValue(0),
                        new StringValue("C++"), new StringValue(""), new IntegerValue(456), new IntegerValue(1),
                        new StringValue("C"), new StringValue(""), new IntegerValue(789), new IntegerValue(2),
                        new StringValue("JavaScript"), new StringValue(""), new IntegerValue(-123), new IntegerValue(3),});
            db.insert("lang", new Value[]{});
            System.out.println("Values inserted");

            final SelectQueryBuilder select = db.createQueryBuilder();
            select.setSourceTable(db.getTables().get("lang"));
            select.addResultColumn(db.createColumnExpression("id", langTable));
            select.addResultColumn(db.createColumnExpression("name", langTable));
            select.addResultColumn(db.createColumnExpression("number", langTable));
            select.addResultColumn(db.createColumnExpression("comment", langTable));

            //id != 1
            select.addCondition(new LiteralComparison(3, ComparisonType.NotEqual, new IntegerValue(1)));

//            select.addCondition(new LiteralComparison(3, ComparisonType.Less, new IntegerValue(2)));

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
