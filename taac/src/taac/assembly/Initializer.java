package taac.assembly;

public abstract class Initializer {
	public abstract long getSize();
	public abstract taac.intermediate.Initializer translate(ParserOutput parsed)
			throws SemanticException;
}
