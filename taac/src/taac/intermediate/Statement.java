package taac.intermediate;

import java.util.ArrayList;

public class Statement {
	public final ArrayList<Instruction> instructions;
	public final String label;

	public Statement(ArrayList<Instruction> instructions, String label) {
		this.instructions = instructions;
		this.label = label;
	}

	public Statement(Instruction instr, String label) {
		this.instructions = new ArrayList<Instruction>();
		instructions.add(instr);
		this.label = label;
	}
}
