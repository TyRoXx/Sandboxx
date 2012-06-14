package taac.assembly;

public class Token {
	public final TokenType type;
	public final String content;
	public final int position;
	public final long line;

	public Token(TokenType type, int position, long line) {
		this.type = type;
		this.content = null;
		this.position = position;
		this.line = line;
	}

	public Token(TokenType type, int position, long line, String content) {
		this.type = type;
		this.content = content;
		this.position = position;
		this.line = line;
	}

	@Override
	public String toString() {
		return type.name + " '" + content + "' (" + position + ")";
	}
}
