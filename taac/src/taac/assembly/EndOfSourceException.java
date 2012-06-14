package taac.assembly;

public class EndOfSourceException extends LexicalException {

	private static final long serialVersionUID = -5897710502878277946L;

	public final TokenType expected;

	public EndOfSourceException(TokenType expected) {
		super("Expected " + expected.name + " instead of end of file");
		this.expected = expected;
	}
}
