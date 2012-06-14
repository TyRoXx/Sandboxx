package taac.assembly;

import java.util.ArrayList;
import java.util.HashMap;

import taac.intermediate.AssignmentOperation;
import taac.intermediate.BinaryOperation;
import taac.intermediate.BooleanOperation;
import taac.intermediate.CharacterValue;
import taac.intermediate.DoubleValue;
import taac.intermediate.IntegerValue;
import taac.intermediate.SignedValue;
import taac.intermediate.StringInitializer;
import taac.intermediate.Value;
import taac.intermediate.ValueInitializer;

public class LineParser {
	private final ArrayList<Token> tokens;
	private final ILineParseCallback callback;
	private int index = 0;

	public LineParser(ArrayList<Token> tokens, ILineParseCallback callback) {
		this.tokens = tokens;
		this.callback = callback;
	}

	public void parse() throws SyntaxException {
		skipLineNumber();

		if (atEnd()) {
			callback.addStatement(new NoOperation(), null);
			return;
		}

		final String label = optionalLabel();

		if (atEnd()) {
			if (label != null) {
				callback.declareVariable(label, null);
				return;
			}
		}

		final Token indicator = current();

		switch (indicator.type) {
		case Key_nop: {
			next();
			expectEnd();
			callback.addStatement(new NoOperation(), label);
			break;
		}

		case Key_con: {
			expectNext("Initializing constant expected");
			final Initializer value = parseInitializer();
			expectEnd();
			callback.declareVariable(label, value);
			break;
		}

		case Key_adr: {
			expectNext();
			final boolean isIndirect = skip(TokenType.Key_adr)
					|| skip(TokenType.Key_val);
			final String name = parseIdentifier();

			if (name.equals("KEY")) {
				if (isIndirect) {
					throw new SyntaxException(
							"KEY cannot be dereferenced with 'adr'");
				}

				skipExpected(TokenType.Assign);
				final Expression argument = parseAlterationArgument();
				callback.addStatement(new Print(argument), label);
			} else {
				final AssignmentOperation type = parseAlterationType();
				final Expression argument = parseAlterationArgument();
				expectEnd();
				final VariableAddress access = new VariableAddress(name);
				final Address altered = (Address) (isIndirect ? new taac.assembly.IndirectAddress(
						access) : access);
				callback.addStatement(new AssignmentStatement(type, altered,
						argument), label);
			}
			break;
		}

		case Key_goto: {
			expectNext();
			final Readable destination = parseJumpDestination();
			expectEnd();
			callback.addStatement(new GoToStatement(destination), label);
			break;
		}

		case Key_stop: {
			next();
			expectEnd();
			callback.addStatement(new Stop(), label);
			break;
		}

		case Key_if: {
			next();
			final BooleanExpression condition = parseBooleanExpression();
			skipExpected(TokenType.Key_then);
			skipExpected(TokenType.Key_goto);
			final Readable destination = parseJumpDestination();
			expectEnd();
			callback.addStatement(new ConditionalJump(destination, condition),
					label);
			break;
		}

		case Key_push: {
			next();
			final Expression argument = parseAlterationArgument();
			expectEnd();
			callback.addStatement(new Push(argument), label);
			break;
		}

		case Key_pull: {
			next();
			final Expression argument = parseAlterationArgument();
			expectEnd();
			callback.addStatement(new Pull(argument), label);
			break;
		}

		/*
		 * case Key_get: { next(); final Readable relativeAddress =
		 * parseOperand(); skipExpected(TokenType.Comma); final Readable
		 * destination = parseOperand(); expectEnd(); callback.addStatement(new
		 * GetFromStack(relativeAddress, destination), label); break; }
		 */

		case Key_getPull: {
			next();
			final Address destination = parseLValue();
			callback.addStatement(new GetPull(destination), label);
			break;
		}

		default:
			callback.addStatement(new NoOperation(), label);
			throw new SyntaxException("Unexpected " + indicator);
		}
	}

	private Address parseLValue() throws SyntaxException {
		skipExpected(TokenType.Key_adr);
		final boolean indirect = skip(TokenType.Key_adr);
		final String name = parseIdentifier();
		final Address address = new VariableAddress(name);
		return (indirect ? new IndirectAddress(address) : address);
	}

	private Readable parseJumpDestination() throws SyntaxException {
		return parseOperand();
	}

	private BooleanExpression parseBooleanExpression() throws SyntaxException {
		final Readable left = parseOperand();
		final BooleanOperation operation = parseBooleanOperation();
		final Readable right = parseOperand();
		return new BooleanExpression(left, right, operation);
	}

	private BooleanOperation parseBooleanOperation() throws SyntaxException {
		final HashMap<TokenType, BooleanOperation> operations = new HashMap<TokenType, BooleanOperation>() {
			private static final long serialVersionUID = -7939330468225493593L;
			{
				put(TokenType.S64_Equal, BooleanOperation.S64equal);
				put(TokenType.S64_Unequal, BooleanOperation.S64unequal);
				put(TokenType.S64_Smaller, BooleanOperation.S64smaller);
				put(TokenType.S64_SmallerEqual,
						BooleanOperation.S64smallerEqual);
				put(TokenType.S64_Greater, BooleanOperation.S64greater);
				put(TokenType.S64_GreaterEqual,
						BooleanOperation.S64greaterEqual);
			}
		};

		final Token operator = current();
		final BooleanOperation operation = operations.get(operator.type);
		if (operation != null) {
			next();
			return operation;
		}

		throw new SyntaxException("Binary operator expected");
	}

	private void expectEnd() throws SyntaxException {
		if (!atEnd()) {
			throw new SyntaxException("End of line expected");
		}
	}

	private Readable parseOperand() throws SyntaxException {
		final Token token = current();
		switch (token.type) {
		case Key_val: {
			expectNext();
			final boolean isIndirect = skip(TokenType.Key_val);
			final String name = parseIdentifier();

			if (name.equals("KEY")) {
				if (isIndirect) {
					throw new SyntaxException(
							"'KEY' cannot be deferenced as a pointer");
				}

				return new Key();
			}

			Address addressable = new VariableAddress(name);
			if (isIndirect) {
				addressable = new IndirectAddress(addressable);
			}
			return new ReadFromAddress(addressable);
		}

		case Key_con: {
			expectNext("Literal expected");
			return parseConstant();
		}

		default:
			throw new SyntaxException("Operand expected");
		}
	}

	private Initializer parseInitializer() throws SyntaxException {
		if (current().type == TokenType.String_Literal) {
			return new SimpleInitializer(new StringInitializer(parseString()));
		} else if (current().type == TokenType.Identifier) {
			final String name = current().content;
			next();
			return new AddressOfInitializer(name);
		} else {
			return new SimpleInitializer(new ValueInitializer(
					parseLiteralValue()));
		}
	}

	private String parseString() {
		final String str = current().content;
		next();
		return str;
	}

	private Readable parseConstant() throws SyntaxException {
		expectNotEnd("Constant expected");
		final Token firstToken = current();
		if (firstToken.type == TokenType.Identifier) {
			next();
			return new AddressOf(firstToken.content);
		} else {
			return new Literal(parseLiteralValue());
		}
	}

	private Expression parseAlterationArgument() throws SyntaxException {
		final Readable left = parseOperand();
		if (atEnd()) {
			return new ReadableExpression(left);
		}

		final BinaryOperation operation = parseBinaryOperation();
		final Readable right = parseOperand();
		return new BinaryExpression(new ReadableExpression(left),
				new ReadableExpression(right), operation);
	}

	private BinaryOperation parseBinaryOperation() throws SyntaxException {
		final HashMap<TokenType, BinaryOperation> operations = new HashMap<TokenType, BinaryOperation>() {
			private static final long serialVersionUID = -7939330468225493593L;
			{
				put(TokenType.S64_Add, BinaryOperation.S64add);
				put(TokenType.S64_Sub, BinaryOperation.S64sub);
				put(TokenType.S64_Mul, BinaryOperation.S64mul);
				put(TokenType.S64_Div, BinaryOperation.S64div);
				put(TokenType.S64_Mod, BinaryOperation.S64mod);
				put(TokenType.S64_And, BinaryOperation.S64and);
				put(TokenType.S64_Or, BinaryOperation.S64or);
				put(TokenType.S64_Xor, BinaryOperation.S64xor);
				put(TokenType.F64_Add, BinaryOperation.F64add);
				put(TokenType.F64_Sub, BinaryOperation.F64sub);
				put(TokenType.F64_Mul, BinaryOperation.F64mul);
				put(TokenType.F64_Div, BinaryOperation.F64div);
				put(TokenType.F64_Mod, BinaryOperation.F64mod);
				put(TokenType.F64_Power, BinaryOperation.F64pow);
			}
		};

		final Token operator = current();
		final BinaryOperation operation = operations.get(operator.type);
		if (operation != null) {
			next();
			return operation;
		}

		throw new SyntaxException("Binary operator expected");
	}

	private AssignmentOperation parseAlterationType() throws SyntaxException {
		final HashMap<TokenType, AssignmentOperation> operations = new HashMap<TokenType, AssignmentOperation>() {
			private static final long serialVersionUID = -1561296596071047046L;
			{
				put(TokenType.Assign, AssignmentOperation.A64copy);
				put(TokenType.S64_Add_Assign, AssignmentOperation.S64add);
				put(TokenType.S64_Sub_Assign, AssignmentOperation.S64sub);
				put(TokenType.S64_Mul_Assign, AssignmentOperation.S64mul);
				put(TokenType.S64_Div_Assign, AssignmentOperation.S64div);
				put(TokenType.S64_Mod_Assign, AssignmentOperation.S64mod);
				put(TokenType.S64_And_Assign, AssignmentOperation.S64and);
				put(TokenType.S64_Or_Assign, AssignmentOperation.S64or);
				put(TokenType.S64_Xor_Assign, AssignmentOperation.S64xor);
				put(TokenType.S64_LShift_Assign,
						AssignmentOperation.S64shiftLeft);
				put(TokenType.S64_RShift_Assign,
						AssignmentOperation.S64shiftRight);
				put(TokenType.U64_RShift_Assign,
						AssignmentOperation.U64shiftRight);
				put(TokenType.F64_Add_Assign, AssignmentOperation.F64add);
				put(TokenType.F64_Sub_Assign, AssignmentOperation.F64sub);
				put(TokenType.F64_Mul_Assign, AssignmentOperation.F64mul);
				put(TokenType.F64_Div_Assign, AssignmentOperation.F64div);
				put(TokenType.F64_Mod_Assign, AssignmentOperation.F64mod);
			}
		};

		final String errorMessage = "Assignment-type operator expected";

		expectNotEnd(errorMessage);
		final Token operator = current();
		final AssignmentOperation operation = operations.get(operator.type);
		if (operation != null) {
			next();
			return operation;
		}

		throw new SyntaxException(errorMessage);
	}

	private String parseIdentifier() throws SyntaxException {
		final Token idToken = current();
		skipExpected(TokenType.Identifier);
		return idToken.content;
	}

	private boolean skip(TokenType type) {
		if (current().type == type) {
			next();
			return true;
		}
		return false;
	}

	private void skipExpected(TokenType type) throws SyntaxException {
		if (!skip(type)) {
			throw new SyntaxException(type.toString() + " expected");
		}
	}

	private Value parseLiteralValue() throws SyntaxException {
		final Token literalToken = current();
		switch (literalToken.type) {
		case S64_Literal_Bin:
		case S64_Literal_Oct:
		case S64_Literal_Dec:
		case S64_Literal_Hex:
		case F64_Literal:
			return parseNumericLiteral();

		case Char_Literal:
			next();
			return new CharacterValue(literalToken.content.charAt(0));

		case String_Literal:
			throw new SyntaxException("A string literal is not allowed here");

		case S64_Sub:
			expectNext("Integer literal expected");
			final SignedValue literal = parseNumericLiteral();
			literal.negate();
			return literal;

		default:
			throw new SyntaxException("Literal expected");
		}
	}

	private SignedValue parseNumericLiteral() throws SyntaxException {
		final Token literalToken = current();

		switch (literalToken.type) {
		case S64_Literal_Bin:
			next();
			return new IntegerValue(Long.parseLong(literalToken.content, 2));

		case S64_Literal_Oct:
			next();
			return new IntegerValue(Long.parseLong(literalToken.content, 8));

		case S64_Literal_Dec:
			next();
			return new IntegerValue(Long.parseLong(literalToken.content, 10));

		case S64_Literal_Hex:
			next();
			return new IntegerValue(Long.parseLong(literalToken.content, 16));
			
		case F64_Literal:
			next();
			return new DoubleValue(Double.parseDouble(literalToken.content));

		default:
			throw new SyntaxException("Integer literal expected");
		}
	}

	private boolean atEnd() {
		return (index == tokens.size());
	}

	private Token current() {
		return tokens.get(index);
	}

	private void next() {
		++index;
	}

	private void expectNext() throws SyntaxException {
		next();

		if (atEnd()) {
			throw new SyntaxException("Unexpected end of line");
		}
	}

	private void expectNext(String message) throws SyntaxException {
		next();

		if (atEnd()) {
			throw new SyntaxException(message);
		}
	}

	private void expectNotEnd(String message) throws SyntaxException {
		if (atEnd()) {
			throw new SyntaxException(message);
		}
	}

	private void skipLineNumber() throws SyntaxException {
		if (atEnd()) {
			return;
		}

		switch (current().type) {
		case S64_Literal_Bin:
		case S64_Literal_Oct:
		case S64_Literal_Dec:
		case S64_Literal_Hex:
			next();
			break;

		default:
			break;
		}
	}

	private String optionalLabel() throws SyntaxException {
		final Token labelTk = current();
		switch (labelTk.type) {
		case Identifier:
			next();
			return labelTk.content;

		default:
			return null;
		}
	}
}
