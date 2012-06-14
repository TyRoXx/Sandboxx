package taac.assembly.representations;

public class Keyword extends Representation {
	public final String content;

	public Keyword(String content) {
		this.content = content;
	}

	@Override
	public void visit(IRepresentationVisitor visitor) {
		visitor.visited(this);
	}
}
