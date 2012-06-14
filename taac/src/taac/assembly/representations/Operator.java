package taac.assembly.representations;

public class Operator extends Representation {
	public final String[] content;

	public Operator(String content) {
		this.content = new String[]{content};
	}
	
	public Operator(String content0, String content1) {
		this.content = new String[]{content0, content1};
	}

	@Override
	public void visit(IRepresentationVisitor visitor) {
		visitor.visited(this);
	}

}
