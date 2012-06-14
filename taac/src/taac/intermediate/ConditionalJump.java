package taac.intermediate;

public class ConditionalJump extends Jump {
	public final BooleanExpression condition;

	public ConditionalJump(Readable destination, BooleanExpression condition) {
		super(destination);

		this.condition = condition;
	}

	@Override
	public void accept(IInstructionVisitor visitor) {
		visitor.visit(this);
	}
}
