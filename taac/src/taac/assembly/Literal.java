package taac.assembly;

import taac.intermediate.Value;

public class Literal extends Readable {
	private final Value value;

	public Literal(Value value) {
		this.value = value;
	}

	@Override
	public taac.intermediate.Readable translate(ParserOutput parsed)
			throws SemanticException {
		return new taac.intermediate.Literal(value);
	}
}
