package taac.intermediate;

public class Stop extends Instruction {
	@Override
	public void accept(IInstructionVisitor visitor) {
		visitor.visit(this);
	}
}
