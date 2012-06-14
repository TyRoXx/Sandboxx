package taac.assembly;

import java.util.ArrayList;

import taac.intermediate.Instruction;

public class ReadableExpression extends Expression {
	private final Readable value;

	public ReadableExpression(Readable value) {
		this.value = value;
	}

	@Override
	public taac.intermediate.Expression translate(ParserOutput parsed,
			ArrayList<Instruction> instructions) throws SemanticException {
		return new taac.intermediate.ReadableExpression(value.translate(parsed));
	}
}
