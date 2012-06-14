package taac.intermediate;

public class DoubleValue extends SignedValue {

	public double value;

	public DoubleValue(double value) {
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
		return "Double " + value;
	}

	public static final DoubleValue type = new DoubleValue(0);
}
