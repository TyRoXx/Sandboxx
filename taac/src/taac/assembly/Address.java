package taac.assembly;

public abstract class Address {
	public abstract taac.intermediate.Address translate(ParserOutput parsed)
			throws SemanticException;
}
