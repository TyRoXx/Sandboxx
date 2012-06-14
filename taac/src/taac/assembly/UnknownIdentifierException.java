package taac.assembly;

public class UnknownIdentifierException extends SemanticException {
	private static final long serialVersionUID = -3826165459975837235L;

	public final String name;

	public UnknownIdentifierException(String name) {
		super("Unknown identifier " + name);
		this.name = name;
	}

}
