package taac.intermediate;

public class Print extends Instruction {
	public final Expression argument;

	public Print(Expression argument) {
		this.argument = argument;
	}

	@Override
	public void accept(IInstructionVisitor visitor) {
		visitor.visit(this);
	}
}
