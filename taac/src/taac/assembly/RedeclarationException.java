package taac.assembly;

public class RedeclarationException extends SyntaxException {
	private static final long serialVersionUID = -3443231406872402926L;

	public RedeclarationException(String name, long otherLine) {
		super("Variable '" + name + "' already declared in line "
				+ (otherLine + 1));
	}
}
