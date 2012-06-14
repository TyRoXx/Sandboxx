package taac.assembly;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import taac.intermediate.Address;
import taac.intermediate.IntermediateProgram;
import taac.intermediate.Statement;
import taac.intermediate.StaticAddress;

public class CodeGenerator {
	private final ParserOutput parsed;

	public static IntermediateProgram generateProgram(ParserOutput parsed)
			throws GeneratorFailedException {
		final CodeGenerator generator = new CodeGenerator(parsed);
		final ArrayList<Statement> statements = generator
				.translateStatements(parsed.statements);
		final Map<Long, taac.intermediate.Initializer> initializations = generator
				.translateInitializations(parsed.initializers);
		return new IntermediateProgram(statements, initializations, parsed.hasStack, parsed.stackPointerAddress);
	}

	private CodeGenerator(ParserOutput parsed) {
		this.parsed = parsed;
	}

	private ArrayList<Statement> translateStatements(
			ArrayList<StatementLine> lines) throws GeneratorFailedException {
		final ArrayList<Statement> result = new ArrayList<Statement>();

		int errorCount = 0;

		int currentLineNumber = 0;
		for (StatementLine line : parsed.statements) {
			try {
				result.add(translateStatement(line));
			} catch (SemanticException e) {
				++errorCount;
				System.out.println("Error (" + (currentLineNumber + 1) + "): "
						+ e.getMessage());
			}

			++currentLineNumber;
		}

		if (errorCount > 0) {
			throw new GeneratorFailedException(errorCount);
		}

		return result;
	}

	private taac.intermediate.Statement translateStatement(StatementLine line)
			throws SemanticException {
		return new taac.intermediate.Statement(
				line.statement.translate(parsed), line.label);
	}

	public taac.intermediate.Readable translateReadable(
			taac.assembly.Readable argument) throws SemanticException {
		return argument.translate(parsed);
	}

	public taac.intermediate.Address translateAddressable(
			taac.assembly.Address altered) throws SemanticException {
		return altered.translate(parsed);
	}

	public Address getAddressOfVariable(String name)
			throws UnknownIdentifierException {
		final Long address = parsed.addressesByName.get(name);
		if (address == null) {
			throw new UnknownIdentifierException(name);
		}

		return new StaticAddress(address);
	}

	private Map<Long, taac.intermediate.Initializer> translateInitializations(
			Map<Long, Initializer> variables)
			throws GeneratorFailedException {
		final HashMap<Long, taac.intermediate.Initializer> result = new HashMap<Long, taac.intermediate.Initializer>();

		int errorCount = 0;

		for (final Entry<Long, taac.assembly.Initializer> pair : parsed.initializers
				.entrySet()) {
			try {
				result.put(pair.getKey(), pair.getValue().translate(parsed));
			} catch (SemanticException e) {
				++errorCount;
				System.out.println(e.getMessage());
			}
		}

		if (errorCount > 0) {
			throw new GeneratorFailedException(errorCount);
		}

		return result;
	}
}
