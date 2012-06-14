package taac.intermediate;

public class IndirectAddress extends Address {
	public final Address addressLocation;

	public IndirectAddress(Address addressLocation) {
		this.addressLocation = addressLocation;
	}

	@Override
	public void visit(IAddressVisitor visitor) {
		visitor.visited(this);
	}
}
