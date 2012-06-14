package taac.intermediate;

public class NonExecutable extends Instruction {
	@Override
	public void accept(IInstructionVisitor visitor) {
		visitor.visit(this);
	}
}
