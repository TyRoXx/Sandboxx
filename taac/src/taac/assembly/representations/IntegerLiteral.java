package taac.assembly.representations;

public class IntegerLiteral extends Representation {

	@Override
	public void visit(IRepresentationVisitor visitor) {
		visitor.visited(this);
	}
}
