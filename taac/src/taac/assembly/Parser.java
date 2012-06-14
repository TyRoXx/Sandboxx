package taac.assembly;

import java.util.ArrayList;

public class Parser {
	private long currentAddress = 0;
	private final ParserOutput output = new ParserOutput();

	public static ParserOutput parse(ArrayList<TokenLine> lines)
			throws ParserFailedException {
		return new Parser(lines).output;
	}

	private Parser(ArrayList<TokenLine> lines) throws ParserFailedException {
		int errorCount = 0;
		int currentLine = 0;
		for (TokenLine line : lines) {
			try {
				parseLine(line);
			} catch (SyntaxException e) {
				++errorCount;
				System.out.println("Error (" + (currentLine + 1) + "): "
						+ e.getMessage());
			}

			++currentLine;
		}

		if (errorCount > 0) {
			throw new ParserFailedException(errorCount);
		}
	}

	private void parseLine(TokenLine line) throws SyntaxException {
		class LineParseCallback implements ILineParseCallback {

			@Override
			public void declareVariable(String name, Initializer value)
					throws RedeclarationException {
				if (name != null) {
					if (name.equals("STACK")) {
						if (value != null) {
							System.out
									.println("You may not initialize 'STACK'");
						}
						value = new AddressOfInitializer("STACK"); // TODO
					}

					if (name.equals("KEY")) {
						output.isKeyDefined = true;
					} else if (name.equals("STACK")) {
						output.stackPointerAddress = currentAddress;
						output.hasStack = true;
					}
				}

				final long variableAddress = currentAddress;

				addStatement(new MemoryStatement(), name);

				if (value != null) {
					output.initializers.put(variableAddress, value);

					for (int i = 1, d = (int) value.getSize(); i < d; ++i) {
						addStatement(new MemoryStatement(), null);
					}
				}
			}

			@Override
			public void addStatement(Statement statement, String label)
					throws RedeclarationException {
				Parser.this.addStatement(statement, label);
			}
		}

		final LineParseCallback callback = new LineParseCallback();
		final LineParser lineParser = new LineParser(line.tokens, callback);
		lineParser.parse();
	}

	private void addStatement(Statement statement, String label)
			throws RedeclarationException {
		{
			final Long predecessor = output.addressesByName.get(label);

			if (predecessor != null) {
				throw new RedeclarationException(label, predecessor);
			}
		}

		output.statements.add(new StatementLine(label, statement));
		if (label != null) {
			output.addressesByName.put(label, currentAddress);
		}
		++currentAddress;
	}
}
