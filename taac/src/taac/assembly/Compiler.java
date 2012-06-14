package taac.assembly;

import java.util.ArrayList;

import taac.intermediate.IntermediateProgram;

public class Compiler {
	public IntermediateProgram compile(String source) throws CompilerFailedException {
		final Lexer scanner = new Lexer(source);
		ArrayList<TokenLine> tokens;
		try {
			tokens = scanner.tokenize();
		} catch (LexerFailedException e) {
			System.out.println(e.getMessage());
			throw new CompilerFailedException(1);
		}

		ParserOutput parsed;
		try {
			parsed = Parser.parse(tokens);
		} catch (ParserFailedException e) {
			System.out.println(e.getMessage());
			throw new CompilerFailedException(e.errorCount);
		}

		IntermediateProgram program;
		try {
			program = CodeGenerator.generateProgram(parsed);
		} catch (GeneratorFailedException e) {
			System.out.println(e.getMessage());
			throw new CompilerFailedException(e.errorCount);
		}
		
		return program;
	}
}
