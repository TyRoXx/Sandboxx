package taac.assembly;

import java.util.ArrayList;

import taac.intermediate.Instruction;

public class Stop extends Statement {
	@Override
	public ArrayList<Instruction> translate(ParserOutput parsed)
			throws SemanticException {
		final ArrayList<Instruction> instructions = new ArrayList<Instruction>();
		instructions.add(new taac.intermediate.Stop());
		return instructions;
	}
}
