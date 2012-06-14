package taac.intermediate;

public class Key extends Readable {
	@Override
	public void accept(IReadableVisitor visitor) {
		visitor.visit(this);
	}
}
