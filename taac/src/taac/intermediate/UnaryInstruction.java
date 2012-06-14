package taac.intermediate;

public class UnaryInstruction extends Instruction {
	public final Address operand;
	public final UnaryOperation operation;

	public UnaryInstruction(Address operand, UnaryOperation operation) {
		this.operand = operand;
		this.operation = operation;
	}

	@Override
	public void accept(IInstructionVisitor visitor) {
		visitor.visit(this);
	}
}
