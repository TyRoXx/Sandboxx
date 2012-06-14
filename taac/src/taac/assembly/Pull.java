package taac.assembly;

import java.util.ArrayList;

import taac.intermediate.Instruction;

public class Pull extends Statement {
	private final Expression count;

	public Pull(Expression count) {
		this.count = count;
	}

	@Override
	public ArrayList<Instruction> translate(ParserOutput parsed)
			throws SemanticException {
		if (!parsed.hasStack) {
			throw new SemanticException("The 'pull' instruction needs a STACK");
		}

		final ArrayList<Instruction> instructions = new ArrayList<Instruction>();
		instructions.add(new taac.intermediate.Pull(count.translate(parsed, instructions)));
		return instructions;
	}
}
