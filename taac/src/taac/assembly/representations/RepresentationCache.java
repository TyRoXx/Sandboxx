package taac.assembly.representations;

import java.util.Map;
import java.util.TreeMap;

import taac.assembly.TokenType;

public class RepresentationCache {
	private Map<String, TokenType> operators = new TreeMap<String, TokenType>();
	private Map<String, TokenType> keywords = new TreeMap<String, TokenType>();
	private int maxOperatorLength = 0;

	public RepresentationCache() {
		buildHelpStructures();
	}

	public int getMaxOperatorLength() {
		return maxOperatorLength;
	}

	public TokenType findOperator(String content) {
		return (TokenType) operators.get(content);
	}

	public TokenType findKeyword(String word) {
		return (TokenType) keywords.get(word);
	}

	private void buildHelpStructures() {
		class RepresentationVisitor implements IRepresentationVisitor {
			private final RepresentationCache cache;
			private final TokenType token;

			public RepresentationVisitor(RepresentationCache cache,
					TokenType token) {
				this.cache = cache;
				this.token = token;
			}

			@Override
			public void visited(CharacterLiteral representation) {
			}

			@Override
			public void visited(FloatingPointLiteral representation) {
			}

			@Override
			public void visited(Identifier representation) {
			}

			@Override
			public void visited(IntegerLiteral representation) {
			}

			@Override
			public void visited(Keyword representation) {
				cache.keywords.put(representation.content, token);
			}

			@Override
			public void visited(Operator representation) {
				for (String op : representation.content) {
					cache.operators.put(op, token);

					if (op.length() > cache.maxOperatorLength) {
						cache.maxOperatorLength = op.length();
					}
				}
			}

			@Override
			public void visited(StringLiteral representation) {
			}

			@Override
			public void visited(EndOfLine endOfLine) {
			}
		}

		for (TokenType token : TokenType.values()) {
			RepresentationVisitor visitor = new RepresentationVisitor(this,
					token);
			token.representation.visit(visitor);
		}
	}
}
