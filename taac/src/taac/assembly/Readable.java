package taac.assembly;

public abstract class Readable {
	public abstract taac.intermediate.Readable translate(ParserOutput parsed)
			throws SemanticException;
}
