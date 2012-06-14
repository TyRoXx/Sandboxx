package taac.intermediate;

public class StaticAddress extends Address {
	public final long value;

	public StaticAddress(long value) {
		this.value = value;
	}

	@Override
	public void visit(IAddressVisitor visitor) {
		visitor.visited(this);
	}
}
