package taac.assembly;

public class Key extends Readable {
	@Override
	public taac.intermediate.Readable translate(ParserOutput parsed)
			throws SemanticException {
		if (!parsed.isKeyDefined) {
			throw new SemanticException("'KEY' must be declared before reading from standard input");
		}
		return new taac.intermediate.Key();
	}
}
