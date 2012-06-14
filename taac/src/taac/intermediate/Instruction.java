package taac.intermediate;

public abstract class Instruction {
	public abstract void accept(IInstructionVisitor visitor);
}
