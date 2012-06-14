package taac.assembly;

import java.util.ArrayList;

import taac.assembly.representations.RepresentationCache;

public class Lexer {
	private final String source;
	private final int length;
	private int position;
	private long currentLine;
	private final RepresentationCache representations = new RepresentationCache();
	private boolean isError = false;

	public Lexer(String source) {
		this.source = source;
		this.length = source.length();
	}

	public ArrayList<TokenLine> tokenize() throws LexerFailedException {
		ArrayList<TokenLine> lines = new ArrayList<TokenLine>();

		position = 0;
		currentLine = 0;
		isError = false;

		for (;;) {
			skipWhitespace();

			if (atEnd()) {
				break;
			}

			lines.add(scanLine());
		}

		if (isError) {
			throw new LexerFailedException();
		}

		return lines;
	}

	private TokenLine scanLine() {
		ArrayList<Token> currentLineTokens = new ArrayList<Token>();

		for (;;) {
			skipWhitespace();
			if (atEnd()) {
				break;
			}

			final char first = current();

			try {
				Token token;
				if (isAlphabetic(first) || (first == '_')) {
					token = scanIdentifier();
				} else if (first == '\n') {
					next();
					break;
				} else if (isDigit(first, 10)) {
					token = scanNumber();
				} else if (first == '\'') {
					token = scanCharacter();
				} else if (first == '\"') {
					token = scanString();
				} else {
					token = scanOperator();
				}

				currentLineTokens.add(token);
			} catch (LexicalException e) {
				isError = true;
				System.out.println(e.getMessage());
			}
		}

		return new TokenLine(currentLineTokens);
	}

	private Token scanOperator() throws LexicalException {
		final int begin = position;
		String content = "";
		TokenType candidate = null;
		int candidateEnd = 0;

		for (int i = 0; !atEnd()
				&& (i < representations.getMaxOperatorLength()); ++i) {
			content += current();

			final TokenType type = representations.findOperator(content);
			if (type != null) {
				candidate = type;
				candidateEnd = (position + 1);
			}

			next();
		}

		if (candidate == null) {
			throw new UnexpectedCharacterException(getLineFromPosition(begin),
					source.charAt(begin));
		}

		position = candidateEnd;
		return new Token(candidate, begin, currentLine, source.substring(begin,
				position));
	}

	private int getLineFromPosition(int begin) {
		int line = 0;
		for (int i = 0; i < begin; ++i) {
			if (source.charAt(i) == '\n') {
				++line;
			}
		}
		return line;
	}

	private Token scanString() throws LexicalException {
		final int begin = position;
		next();

		StringBuilder content = new StringBuilder();

		for (;;) {
			if (atEnd()) {
				throw new EndOfSourceException(TokenType.String_Literal);
			}

			final char c = current();
			if (c == '\"') {
				next();
				break;
			}

			content.append(c);
			next();
		}

		return new Token(TokenType.String_Literal, begin, currentLine,
				content.toString());
	}

	private Token scanCharacter() throws LexicalException {
		final int begin = position;
		next();

		if (atEnd()) {
			throw new EndOfSourceException(TokenType.Char_Literal);
		}

		final char c = current();
		next();

		if (atEnd() || current() != '\'') {
			throw new EndOfSourceException(TokenType.Char_Literal);
		}

		next();
		return new Token(TokenType.Char_Literal, begin, currentLine, "" + c);
	}

	private Token scanNumber() throws LexicalException {
		int base = 10;
		TokenType type = TokenType.S64_Literal_Dec;

		if ((current() == '0') && hasNext()) {
			switch (source.charAt(position + 1)) {
			case 'b':
				next();
				base = 2;
				type = TokenType.S64_Literal_Bin;
				next();
				if (atEnd()) {
					throw new EndOfSourceException(TokenType.S64_Literal_Bin);
				}
				break;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				next();
				base = 8;
				type = TokenType.S64_Literal_Oct;
				break;

			case 'x':
				next();
				base = 16;
				type = TokenType.S64_Literal_Hex;
				next();
				if (atEnd()) {
					throw new EndOfSourceException(TokenType.S64_Literal_Hex);
				}
				break;
			}
		}

		final int begin = position;

		for (;;) {
			next();
			if (atEnd()) {
				break;
			}
			
			if ((type == TokenType.S64_Literal_Dec) &&
					(current() == '.'))
			{
				type = TokenType.F64_Literal;
			}
			else if (!isDigit(current(), base))
			{
				break;
			}
		}

		final int end = position;
		return new Token(type, begin, currentLine, source.substring(begin, end));
	}

	private Token scanIdentifier() {
		final int begin = position;

		while (!atEnd()
				&& (isAlphabetic(current()) || isDigit(current(), 10) || (current() == '_'))) {
			next();
		}

		final String content = source.substring(begin, position);

		TokenType type = representations.findKeyword(content);
		if (type == null) {
			type = TokenType.Identifier;
		}
		return new Token(type, begin, currentLine, content);
	}

	private char current() {
		return source.charAt(position);
	}

	private void next() {
		++position;
	}

	private boolean atEnd() {
		return (position == length);
	}

	private boolean hasNext() {
		return (position + 1) < length;
	}

	private void skipWhitespace() {
		while (!atEnd() && (skipWhitespaceCharacter() || skipComment())) {
		}
	}

	private boolean skipWhitespaceCharacter() {
		if (isWhitespaceCharacter(current())) {
			next();
			return true;
		}

		return false;
	}

	private boolean skipComment() {
		if (skip("//")) {
			while (!atEnd() && (current() != '\n')) {
				next();
			}

			return true;
		}

		return false;
	}

	private boolean skip(String string) {
		int p = position;
		for (char c : string.toCharArray()) {
			if (source.charAt(p) != c) {
				return false;
			}
			
			++p;
		}

		position = p;
		return true;
	}

	private static boolean isWhitespaceCharacter(char c) {
		switch (c) {
		case ' ':
		case '\r':
		case '\t':
			return true;

		default:
			return false;
		}
	}

	private static boolean isAlphabetic(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}

	private static boolean isDigit(char c, int base) {
		if (c >= '0' && c < ('0' + Math.min(base, 10))) {
			return true;
		}

		if (base <= 10) {
			return false;
		}

		base -= 10;

		final char lower = Character.toLowerCase(c);
		return (lower >= 'a') && (lower < ('a' + base));
	}
}
