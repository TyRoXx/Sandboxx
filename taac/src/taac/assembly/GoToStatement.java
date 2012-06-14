package taac.assembly;

import java.util.ArrayList;

import taac.intermediate.Instruction;
import taac.intermediate.Jump;

public class GoToStatement extends Statement {
	private final Readable destination;

	public GoToStatement(Readable destination) {
		this.destination = destination;
	}

	@Override
	public ArrayList<Instruction> translate(ParserOutput parsed)
			throws SemanticException {
		ArrayList<Instruction> instructions = new ArrayList<Instruction>();
		instructions.add(new Jump(destination.translate(parsed)));
		return instructions;
	}
}
