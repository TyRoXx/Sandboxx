package taac.assembly;

import java.util.ArrayList;

import taac.intermediate.Instruction;
import taac.intermediate.NonExecutable;

public class MemoryStatement extends Statement {
	@Override
	public ArrayList<Instruction> translate(ParserOutput parsed)
			throws SemanticException {
		ArrayList<Instruction> instructions = new ArrayList<Instruction>();
		instructions.add(new NonExecutable());
		return instructions;
	}
}
