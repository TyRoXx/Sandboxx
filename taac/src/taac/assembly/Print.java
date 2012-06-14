package taac.assembly;

import java.util.ArrayList;

import taac.intermediate.Instruction;

public class Print extends Statement {
	private final Expression argument;

	public Print(Expression argument) {
		this.argument = argument;
	}

	@Override
	public ArrayList<Instruction> translate(ParserOutput parsed)
			throws SemanticException {
		if (!parsed.isKeyDefined) {
			throw new SemanticException("'KEY' must be declared before writing to standard output");
		}

		ArrayList<Instruction> instructions = new ArrayList<Instruction>();
		instructions.add(new taac.intermediate.Print(argument.translate(parsed,
				instructions)));
		return instructions;
	}

}
