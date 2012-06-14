package taac.assembly.representations;

public class EndOfLine extends Representation {
	@Override
	public void visit(IRepresentationVisitor visitor) {
		visitor.visited(this);
	}
}
