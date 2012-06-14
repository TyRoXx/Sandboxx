package taac.assembly.representations;

public class FloatingPointLiteral extends Representation {

	@Override
	public void visit(IRepresentationVisitor visitor) {
		visitor.visited(this);
	}
}
