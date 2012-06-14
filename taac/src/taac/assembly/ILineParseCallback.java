package taac.assembly;

public interface ILineParseCallback {
	void declareVariable(String name, Initializer value) throws RedeclarationException;

	void addStatement(Statement statement, String label) throws RedeclarationException;
}
