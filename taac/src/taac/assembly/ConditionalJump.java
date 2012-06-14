package taac.assembly;

import java.util.ArrayList;

import taac.intermediate.Instruction;

public class ConditionalJump extends Statement {
	private final Readable destination;
	private final BooleanExpression condition;

	public ConditionalJump(Readable destination, BooleanExpression condition) {
		this.destination = destination;
		this.condition = condition;
	}

	@Override
	public ArrayList<Instruction> translate(ParserOutput parsed)
			throws SemanticException {
		ArrayList<Instruction> instructions = new ArrayList<Instruction>();
		instructions.add(new taac.intermediate.ConditionalJump(destination
				.translate(parsed), condition.translate(parsed)));
		return instructions;
	}
}
