package taac.assembly;

import java.util.ArrayList;

import taac.intermediate.Instruction;

public class Push extends Statement {
	private final Expression argument;

	public Push(Expression argument) {
		this.argument = argument;
	}

	@Override
	public ArrayList<Instruction> translate(ParserOutput parsed)
			throws SemanticException {
		if (!parsed.hasStack) {
			throw new SemanticException("The 'push' instruction needs a STACK");
		}

		final ArrayList<Instruction> instructions = new ArrayList<Instruction>();
		
		instructions.add(new taac.intermediate.Push(
				argument.translate(parsed, instructions)));

		return instructions;
	}
}
