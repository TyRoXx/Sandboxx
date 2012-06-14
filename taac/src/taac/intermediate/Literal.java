package taac.intermediate;

public class Literal extends Readable {
	public final Value value;

	public Literal(Value value) {
		this.value = value;
	}

	@Override
	public void accept(IReadableVisitor visitor) {
		visitor.visit(this);
	}
}
