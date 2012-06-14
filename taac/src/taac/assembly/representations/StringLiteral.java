package taac.assembly.representations;

public class StringLiteral extends Representation {

	@Override
	public void visit(IRepresentationVisitor visitor) {
		visitor.visited(this);
	}
}
