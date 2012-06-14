package taac.intermediate;

public class IntegerValue extends SignedValue {
	public long value;

	public IntegerValue(long value) {
		this.value = value;
	}

	@Override
	public void visit(IValueVisitor visitor) {
		visitor.visited(this);
	}

	@Override
	public void negate() {
		value = -value;
	}

	@Override
	public String toString() {
		return "Int " + value;
	}

	public static final IntegerValue type = new IntegerValue(0);
}
