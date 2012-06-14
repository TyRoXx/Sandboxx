package taac.intermediate;

public class GetPull extends Instruction {
	public final Address destination;
	
	public GetPull(Address destination) {
		this.destination = destination;
	}

	@Override
	public void accept(IInstructionVisitor visitor) {
		visitor.visit(this);
	}
}
