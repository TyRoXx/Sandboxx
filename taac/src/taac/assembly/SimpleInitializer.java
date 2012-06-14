package taac.assembly;

public class SimpleInitializer extends Initializer {
	private final taac.intermediate.Initializer value;

	public SimpleInitializer(taac.intermediate.Initializer value) {
		this.value = value;
	}

	@Override
	public long getSize() {
		return value.getSize();
	}

	@Override
	public taac.intermediate.Initializer translate(ParserOutput parsed)
			throws SemanticException {
		return value;
	}
}
