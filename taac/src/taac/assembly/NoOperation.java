package taac.assembly;

import java.util.ArrayList;

import taac.intermediate.Instruction;

public class NoOperation extends Statement {

	@Override
	public String toString() {
		return "nop";
	}

	@Override
	public ArrayList<Instruction> translate(ParserOutput parsed)
			throws SemanticException {
		return new ArrayList<Instruction>();
	}
}
