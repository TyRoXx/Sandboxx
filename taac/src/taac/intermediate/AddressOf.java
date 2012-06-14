package taac.intermediate;

public class AddressOf extends Readable {
	public final long address;

	public AddressOf(long address) {
		this.address = address;
	}

	@Override
	public void accept(IReadableVisitor visitor) {
		visitor.visit(this);
	}
}
