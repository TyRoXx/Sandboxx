package taac.intermediate;

public class AssignmentInstruction extends Instruction {
	public final Address left;
	public final Expression right;
	public final AssignmentOperation operation;

	public AssignmentInstruction(Address left, Expression right,
			AssignmentOperation operation) {
		this.left = left;
		this.right = right;
		this.operation = operation;
	}

	@Override
	public void accept(IInstructionVisitor visitor) {
		visitor.visit(this);
	}
}
