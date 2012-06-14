package taac.intermediate;

public class AddressValue extends Value {
	public final long address;

	public AddressValue(long address) {
		this.address = address;
	}

	@Override
	public void visit(IValueVisitor visitor) {
		visitor.visited(this);
	}
}
