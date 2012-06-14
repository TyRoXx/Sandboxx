package taac.intermediate;

public class ReadableFromAddress extends Readable {
	public final Address location;

	public ReadableFromAddress(Address location) {
		this.location = location;
	}

	@Override
	public void accept(IReadableVisitor visitor) {
		visitor.visit(this);
	}
}
