package taac.assembly.representations;

public class CharacterLiteral extends Representation {

	@Override
	public void visit(IRepresentationVisitor visitor) {
		visitor.visited(this);
	}

}
