package taac.assembly;

import java.util.ArrayList;

import taac.intermediate.Instruction;

public class GetPull extends Statement {
	private final Address destination;
	
	public GetPull(Address destination) {
		this.destination = destination;
	}

	@Override
	public ArrayList<Instruction> translate(ParserOutput parsed)
			throws SemanticException {
		final ArrayList<Instruction> instructions = new ArrayList<Instruction>();
		instructions.add(new taac.intermediate.GetPull(destination.translate(parsed)));
		return instructions;
	}
}
