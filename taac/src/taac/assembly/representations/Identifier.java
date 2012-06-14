package taac.assembly.representations;

public class Identifier extends Representation {

	@Override
	public void visit(IRepresentationVisitor visitor) {
		visitor.visited(this);
	}
}
