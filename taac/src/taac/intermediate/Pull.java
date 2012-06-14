package taac.intermediate;

public class Pull extends Instruction {
	public final Expression count;

	public Pull(Expression count) {
		this.count = count;
	}

	@Override
	public void accept(IInstructionVisitor visitor) {
		visitor.visit(this);
	}
}
