package taac.assembly;

public class UnexpectedCharacterException extends LexicalException {

	private static final long serialVersionUID = 5395668359200364212L;

	public UnexpectedCharacterException(int line, char c) {
		super("Unexpected character '" + c + "' in line " + (line + 1));
	}
}
