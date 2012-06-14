package taac.assembly.representations;

public interface IRepresentationVisitor {
	void visited(CharacterLiteral representation);

	void visited(FloatingPointLiteral representation);

	void visited(Identifier representation);

	void visited(IntegerLiteral representation);

	void visited(Keyword representation);

	void visited(Operator representation);

	void visited(StringLiteral representation);

	void visited(EndOfLine endOfLine);
}
