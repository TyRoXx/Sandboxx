package taac.assembly;

import java.util.ArrayList;

import taac.intermediate.BinaryOperation;
import taac.intermediate.Instruction;

public class BinaryExpression extends Expression {
	private final Expression left, right;
	private final BinaryOperation operation;

	public BinaryExpression(Expression left, Expression right,
			BinaryOperation operation) {
		this.left = left;
		this.right = right;
		this.operation = operation;
	}

	@Override
	public taac.intermediate.Expression translate(ParserOutput parsed,
			ArrayList<Instruction> instructions) throws SemanticException {
		return new taac.intermediate.BinaryExpression(left.translate(parsed,
				instructions), right.translate(parsed, instructions), operation);
	}
}
