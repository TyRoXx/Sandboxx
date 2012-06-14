package taac.intermediate;

public class ValueInitializer extends Initializer {
	public final Value value;

	public ValueInitializer(Value value) {
		this.value = value;
	}

	@Override
	public long getSize() {
		return 1;
	}

	@Override
	public void accept(IInitializerVisitor visitor) {
		visitor.visit(this);
	}
}
