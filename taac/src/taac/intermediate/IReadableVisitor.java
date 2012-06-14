package taac.intermediate;

public interface IReadableVisitor {
	void visit(Literal readable);

	void visit(ReadableFromAddress readable);

	void visit(AddressOf addressOf);

	void visit(Key key);
}
