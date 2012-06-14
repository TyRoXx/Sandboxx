package taac.assembly;

import taac.intermediate.BooleanOperation;

public class BooleanExpression {
	private final Readable left, right;
	private final BooleanOperation operation;

	public BooleanExpression(Readable left, Readable right,
			BooleanOperation operation) {
		this.left = left;
		this.right = right;
		this.operation = operation;
	}

	public taac.intermediate.BooleanExpression translate(ParserOutput parsed)
			throws SemanticException {
		return new taac.intermediate.BooleanExpression(left.translate(parsed),
				right.translate(parsed), operation);
	}
}
