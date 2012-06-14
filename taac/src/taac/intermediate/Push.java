package taac.intermediate;

public class Push extends Instruction {
	public final Expression element;

	public Push(Expression element) {
		this.element = element;
	}

	@Override
	public void accept(IInstructionVisitor visitor) {
		visitor.visit(this);
	}
}
