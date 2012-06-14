package taac.intermediate;

public class AnyValue extends Value {

	private AnyValue() {
	}

	public static final AnyValue type = new AnyValue();

	@Override
	public void visit(IValueVisitor visitor) {
		visitor.visited(this);
	}

}
