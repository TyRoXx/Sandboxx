package taac.intermediate;

public class StringInitializer extends Initializer {
	public final String value;

	public StringInitializer(String value) {
		this.value = value;
	}

	@Override
	public long getSize() {
		return value.length();
	}

	@Override
	public void accept(IInitializerVisitor visitor) {
		visitor.visit(this);
	}
}
