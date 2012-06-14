package taac.assembly;

import java.util.ArrayList;

import taac.intermediate.AssignmentInstruction;
import taac.intermediate.AssignmentOperation;
import taac.intermediate.Instruction;

public class AssignmentStatement extends Statement {
	public final AssignmentOperation operation;
	public final Address altered;
	public final Expression argument;

	public AssignmentStatement(AssignmentOperation operation, Address altered,
			Expression argument) {
		this.operation = operation;
		this.altered = altered;
		this.argument = argument;
	}

	@Override
	public ArrayList<Instruction> translate(ParserOutput parsed)
			throws SemanticException {
		ArrayList<Instruction> instructions = new ArrayList<Instruction>();
		instructions.add(new AssignmentInstruction(altered.translate(parsed),
				argument.translate(parsed, instructions), operation));
		return instructions;
	}
}
