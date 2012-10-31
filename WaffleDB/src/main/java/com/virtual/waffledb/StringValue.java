package com.virtual.waffledb;

/**
 *
 * @author virtual
 */
public class StringValue implements Value {
    public final String value;
    
    public StringValue(String value) {
        this.value = value;
    }

    public void accept(ValueVisitor visitor) {
        visitor.visit(this);
    }

    @Override
    public String toString() {
        return value;
    }
    
    @Override
    public int hashCode() {
        return value.hashCode();
    }

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final StringValue other = (StringValue) obj;
        return this.value.equals(other.value);
    }
}
