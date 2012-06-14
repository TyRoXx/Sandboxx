package taac.assembly;

import java.util.ArrayList;

public abstract class Statement {
	public abstract ArrayList<taac.intermediate.Instruction> translate(
			ParserOutput parsed) throws SemanticException;
}
