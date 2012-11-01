package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public class LiteralComparison implements Condition {

    public final int columnIndex;
    public final ComparisonType type;
    public final Value literal;

    public LiteralComparison(int columnIndex, ComparisonType type, Value literal) {
        this.columnIndex = columnIndex;
        this.type = type;
        this.literal = literal;
    }
}
