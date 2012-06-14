package taac.assembly;

import java.util.ArrayList;

import taac.intermediate.Instruction;

public abstract class Expression {
	public abstract taac.intermediate.Expression translate(ParserOutput parsed,
			ArrayList<Instruction> instructions) throws SemanticException;
}
