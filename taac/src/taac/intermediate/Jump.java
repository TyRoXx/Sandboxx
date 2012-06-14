package taac.intermediate;

public class Jump extends Instruction {
	public final Readable destination;

	public Jump(Readable destination) {
		this.destination = destination;
	}

	@Override
	public void accept(IInstructionVisitor visitor) {
		visitor.visit(this);
	}
}
