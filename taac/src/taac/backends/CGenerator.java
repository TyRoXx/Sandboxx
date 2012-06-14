package taac.backends;

import java.util.HashMap;
import java.util.Map;

import taac.intermediate.Address;
import taac.intermediate.AddressOf;
import taac.intermediate.AddressValue;
import taac.intermediate.AnyValue;
import taac.intermediate.AssignmentInstruction;
import taac.intermediate.AssignmentOperation;
import taac.intermediate.BinaryExpression;
import taac.intermediate.BinaryOperation;
import taac.intermediate.BooleanExpression;
import taac.intermediate.BooleanOperation;
import taac.intermediate.CharacterValue;
import taac.intermediate.ConditionalJump;
import taac.intermediate.DoubleValue;
import taac.intermediate.GetPull;
import taac.intermediate.IndirectAddress;
import taac.intermediate.Expression;
import taac.intermediate.IAddressVisitor;
import taac.intermediate.IExpressionVisitor;
import taac.intermediate.IInitializerVisitor;
import taac.intermediate.IInstructionVisitor;
import taac.intermediate.IReadableVisitor;
import taac.intermediate.IValueVisitor;
import taac.intermediate.Initializer;
import taac.intermediate.Instruction;
import taac.intermediate.IntegerValue;
import taac.intermediate.IntermediateProgram;
import taac.intermediate.Jump;
import taac.intermediate.Key;
import taac.intermediate.Literal;
import taac.intermediate.NonExecutable;
import taac.intermediate.Print;
import taac.intermediate.Pull;
import taac.intermediate.Push;
import taac.intermediate.Readable;
import taac.intermediate.ReadableExpression;
import taac.intermediate.ReadableFromAddress;
import taac.intermediate.Statement;
import taac.intermediate.StaticAddress;
import taac.intermediate.Stop;
import taac.intermediate.StringInitializer;
import taac.intermediate.UnaryInstruction;
import taac.intermediate.Value;
import taac.intermediate.ValueInitializer;

public class CGenerator {
	private static final String jumpLabelPrefix = "j_";
	private static final String memoryCellTypeName = "memory_cell_t";
	private static final String s64ValueFieldName = "i";
	private static final String f64ValueFieldName = "f";
	private static final String s64TypeName = "int64_t";
	private static final String f64TypeName = "double";
	private static final String memoryArrayName = "g_memory";
	private static final String currentMemorySize = "g_memorySize";
	private static final String resizeMemoryFunction = "resize_memory";
	private static final String cleanUpMemoryFunction = "cleanup_memory";
	private static final String checkDivisorFunction = "check_divisor";
	private static final String memoryAccessFunction = "Mem";
	private static final String stdFunctionPrefix = "std3aa_";
	private static final String stdOutputName = stdFunctionPrefix + "out";
	private static final String stdInputName = stdFunctionPrefix + "in";
	private static final String programCounterVariable = "pc";
	private static final String pcJumpLabelName = "pc_jump";
	private static final String namedAddressPrefix = "a_";
	private static final String exitJumpLabel = "l3aa_stop";

	private final IntermediateProgram program;
	private final boolean hasRuntimeChecks;
	private final long memorySize;
	private final boolean hasAutoResizeStack;

	public CGenerator(IntermediateProgram program, boolean hasRuntimeChecks,
			long initialStackSize, boolean hasAutoResizeStack) {
		this.program = program;
		this.hasRuntimeChecks = hasRuntimeChecks;
		this.memorySize = Math
				.max(Math.max(program.code.size(), 1),
						program.hasStack ? (program.stackPointerAddress + 1 + initialStackSize)
								: Long.MIN_VALUE);
		this.hasAutoResizeStack = hasAutoResizeStack;
	}

	public String generate() {
		return generateIncludes() + generateHelperFunctions()
				+ generateMemory() + generateNamedAddressConstants()
				+ generateMain();
	}

	private String generateIncludes() {
		return "#include <stdio.h>\n" + "#include <assert.h>\n"
				+ "#include <stdint.h>\n" + "#include <stdlib.h>\n"
				+ "#include <stddef.h>\n\n";
	}

	private String generateHelperFunctions() {
		final String stdInOut = "static void " + stdOutputName + "("
				+ s64TypeName + " c) {\n"
				+ "\tputc((char)c, stdout);\n}\n\nstatic " + s64TypeName + " "
				+ stdInputName + "(void) {\n" + "\treturn getc(stdin);\n}\n\n";

		String checkDivisor;
		if (hasRuntimeChecks) {
			checkDivisor = "static "
					+ s64TypeName
					+ " "
					+ checkDivisorFunction
					+ "("
					+ s64TypeName
					+ " divisor) {\n"
					+ "\tif (divisor == 0) { fprintf(stderr, \"division by zero\\n\"); exit(1); }\n"
					+ "\treturn divisor;\n" + "}\n\n";
		} else {
			checkDivisor = "";
		}

		return stdInOut + checkDivisor;
	}

	private String generateMemory() {
		final String memoryType = "typedef union {\n\t" + s64TypeName + " "
				+ s64ValueFieldName + ";\n\t" + f64TypeName + " "
				+ f64ValueFieldName + ";\n" + "} " + memoryCellTypeName
				+ ";\n\n";

		String memoryPtr = "static " + memoryCellTypeName;
		if (hasAutoResizeStack) {
			memoryPtr += " *" + memoryArrayName + " = 0;\nstatic "
					+ s64TypeName + " " + currentMemorySize + " = "
					+ memorySize + ";\n\n";
		} else {
			memoryPtr += " " + memoryArrayName + "[" + memorySize
					+ "] = {{0}};\nstatic const " + s64TypeName + " "
					+ currentMemorySize + " = " + memorySize + ";\n\n";
		}

		String resizeMemory = "static void " + resizeMemoryFunction
				+ "(void) {\n";

		if (hasAutoResizeStack) {
			resizeMemory += "\t"
					+ memoryArrayName
					+ " = realloc("
					+ memoryArrayName
					+ ", (size_t)(sizeof("
					+ memoryCellTypeName
					+ ") * "
					+ currentMemorySize
					+ "));\n"
					+ "\tif (!"
					+ memoryArrayName
					+ ") { fputs(\"memory allocation failed\\n\", stderr); exit(1); }\n";
		}

		resizeMemory += "}\n\n";

		String cleanUpMemory = "static void " + cleanUpMemoryFunction
				+ "(void) {\n";

		if (hasAutoResizeStack) {
			cleanUpMemory += "\tfree(" + memoryArrayName + ");\n";
		}

		cleanUpMemory += "}\n\n";

		String memoryAccess = "static " + memoryCellTypeName + " *"
				+ memoryAccessFunction + "_impl(" + s64TypeName
				+ " address) {\n";

		if (hasAutoResizeStack) {
			memoryAccess += "\tif (address >= " + currentMemorySize + ") {\n"
					+ "\t\t" + currentMemorySize + " = address + 1;\n" + "\t\t"
					+ resizeMemoryFunction + "();\n" + "\t}\n";
		} else if (hasRuntimeChecks) {
			memoryAccess += "\tassert(address >= 0 && address < " + memorySize
					+ ");\n";
		}

		memoryAccess += "\treturn " + memoryArrayName + " + address;\n" + "}\n"
				+ "#define " + memoryAccessFunction + "(addr) (*"
				+ memoryAccessFunction + "_impl(addr))\n\n";

		return memoryType + memoryPtr + resizeMemory + cleanUpMemory
				+ memoryAccess;
	}

	private String generateNamedAddressConstants() {
		StringBuilder str = new StringBuilder();

		str.append("enum {\n");
		long address = 0;
		for (Statement statement : program.code) {
			final String label = statement.label;

			if (label != null) {
				str.append("\t" + namedAddressPrefix + label + " = " + address
						+ ",\n");
			}

			++address;
		}
		str.append("};\n\n");

		return str.toString();
	}

	private String generateMain() {
		return "int main() {\n" + generateMainBody() + exitJumpLabel + ":\n\t"
				+ cleanUpMemoryFunction + "();\n\treturn 0;\n}\n";
	}

	private String generateMainBody() {
		Indentation bodyIndent = new Indentation().more();
		StringBuilder body = new StringBuilder();

		body.append(bodyIndent + s64TypeName + " " + programCounterVariable
				+ " = 0;\n");

		body.append(bodyIndent + resizeMemoryFunction + "();\n\n");

		body.append(generateInitialMemoryLayout(program.initialMemoryValues,
				bodyIndent));

		body.append("\n" + bodyIndent + "/*program code*/\n");
		body.append(pcJumpLabelName + ": switch (pc)\n");
		body.append(bodyIndent + "{\n");
		final Indentation caseIndent = bodyIndent;
		final Indentation instrIndent = caseIndent.more();

		long pc = 0;
		for (Statement line : program.code) {
			body.append(caseIndent + "case " + pc + ": ");
			// body.append("printf(\"" + pc + "\\n\"); ");
			body.append(generateLine(line, instrIndent));

			++pc;
		}

		body.append(bodyIndent + "}\n");
		return body.toString();
	}

	private String generateLine(Statement line, Indentation bodyIndent) {
		StringBuilder str = new StringBuilder();

		if (line.label != null) {
			str.append(jumpLabelPrefix + line.label + ": ");
		}

		boolean writeSpace = false;
		for (Instruction instr : line.instructions) {
			if (writeSpace) {
				str.append(" ");
			} else {
				writeSpace = true;
			}
			str.append(generateInstruction(instr));
		}

		if (line.instructions.isEmpty()) {
			str.append("(void)0;");
		}

		str.append("\n");
		return str.toString();
	}

	private String generateInitialMemoryLayout(Map<Long, Initializer> map,
			Indentation bodyIndent) {
		StringBuilder str = new StringBuilder();
		str.append(bodyIndent + "/*initial memory*/\n");

		for (Map.Entry<Long, Initializer> value : map.entrySet()) {
			final long address = value.getKey();
			final Initializer constant = value.getValue();

			str.append(generateInitialization(address, constant, bodyIndent));
		}

		return str.toString();
	}

	private String generateInitialization(final long address,
			Initializer initializer, final Indentation indent) {
		class InitializerVisitor implements IInitializerVisitor {
			public String str;

			@Override
			public void visit(StringInitializer stringInitializer) {
				str = "";
				final String value = stringInitializer.value;
				for (int i = 0, d = value.length(); i < d; ++i) {
					str += indent
							+ generateAssignment(new AssignmentInstruction(
									new StaticAddress(address + i),
									new ReadableExpression(
											new Literal(new CharacterValue(
													value.charAt(i)))),
									AssignmentOperation.S64copy)) + "\n";
				}
			}

			@Override
			public void visit(ValueInitializer valueInitializer) {
				str = indent
						+ generateAssignment(new AssignmentInstruction(
								new StaticAddress(address),
								new ReadableExpression(new Literal(
										valueInitializer.value)),
								AssignmentOperation.A64copy)) + "\n";
			}
		}

		final InitializerVisitor visitor = new InitializerVisitor();
		initializer.accept(visitor);
		return visitor.str;
	}

	private String generateInstruction(Instruction instruction) {
		class InstructionVisitor implements IInstructionVisitor {
			public String str;

			@Override
			public void visit(AssignmentInstruction binary) {
				str = generateAssignment(binary);
			}

			@Override
			public void visit(Jump jump) {
				str = generateJump(jump.destination);
			}

			@Override
			public void visit(ConditionalJump conditionalJump) {
				str = "if ("
						+ generateBooleanExpression(conditionalJump.condition)
						+ ") { " + generateJump(conditionalJump.destination)
						+ " }";
			}

			@Override
			public void visit(UnaryInstruction unaryInstruction) {
				str = generateUnaryInstruction(unaryInstruction);
			}

			@Override
			public void visit(Stop stop) {
				str = "goto " + exitJumpLabel + ";";
			}

			@Override
			public void visit(NonExecutable nonExecutable) {
				if (hasRuntimeChecks) {
					str = "assert(!\"not executable\");";
				} else {
					str = "(void)0; /*not executable*/";
				}
			}

			@Override
			public void visit(Print print) {
				str = stdOutputName
						+ "("
						+ generateExpression(print.argument,
								new Value[] { IntegerValue.type }) + ");";
			}

			@Override
			public void visit(Push push) {
				final StaticAddress stackPtrAddress = new StaticAddress(
						program.stackPointerAddress);

				str = generateAssignment(new AssignmentInstruction(
						stackPtrAddress, new ReadableExpression(new Literal(
								new IntegerValue(1))),
						AssignmentOperation.S64add))
						+ " "
						+ generateAssignment(new AssignmentInstruction(
								new IndirectAddress(stackPtrAddress),
								push.element, AssignmentOperation.A64copy));
			}

			@Override
			public void visit(Pull pull) {
				final StaticAddress stackPtrAddress = new StaticAddress(
						program.stackPointerAddress);

				str = generateAssignment(new AssignmentInstruction(
						stackPtrAddress, pull.count, AssignmentOperation.S64sub));
			}

			@Override
			public void visit(GetPull getPull) {
				final StaticAddress stackPtrAddress = new StaticAddress(
						program.stackPointerAddress);

				str = generateAssignment(new AssignmentInstruction(
						getPull.destination, new ReadableExpression(
								new ReadableFromAddress(new IndirectAddress(
										stackPtrAddress))),
						AssignmentOperation.A64copy))
						+ " "
						+ generateAssignment(new AssignmentInstruction(
								stackPtrAddress, new ReadableExpression(
										new Literal(new IntegerValue(1))),
								AssignmentOperation.S64sub))

				;
			}
		}

		InstructionVisitor visitor = new InstructionVisitor();
		instruction.accept(visitor);
		return visitor.str;
	}

	private String generateJump(Readable destination) {
		if (destination instanceof AddressOf) {
			final AddressOf addressOf = (AddressOf) destination;
			return "goto " + getJumpLabelName(addressOf.address) + ";";
		} else {
			final String destinationStr = generateReadable(destination,
					new Value[] { IntegerValue.type });

			return programCounterVariable + " = " + destinationStr + "; goto "
					+ pcJumpLabelName + ";";
		}
	}

	private String getJumpLabelName(long address) {
		final String label = program.code.get((int) address).label;
		assert (label != null);
		return jumpLabelPrefix + label;
	}

	private String generateAssignment(AssignmentInstruction instruction) {
		final HashMap<AssignmentOperation, String> operators = new HashMap<AssignmentOperation, String>() {
			private static final long serialVersionUID = 1802915878572550172L;
			{
				put(AssignmentOperation.A64copy, "=");
				put(AssignmentOperation.S64copy, "=");
				put(AssignmentOperation.S64add, "+=");
				put(AssignmentOperation.S64sub, "-=");
				put(AssignmentOperation.S64mul, "*=");
				put(AssignmentOperation.S64div, "/=");
				put(AssignmentOperation.S64mod, "%=");
				put(AssignmentOperation.S64and, "&=");
				put(AssignmentOperation.S64or, "|=");
				put(AssignmentOperation.S64xor, "^=");
				put(AssignmentOperation.S64shiftLeft, "<<=");
				// put(AssignmentOperation.S64shiftRight, ">>=");
				put(AssignmentOperation.U64shiftRight, ">>=");
				put(AssignmentOperation.F64add, "+=");
				put(AssignmentOperation.F64sub, "-=");
				put(AssignmentOperation.F64mul, "*=");
				put(AssignmentOperation.F64div, "/=");
				// put(AssignmentOperation.F64mod, "%=");
			}
		};

		final AssignmentOperation operation = instruction.operation;
		final String operator = operators.get(operation);
		if (operator == null) {
			throw new UnsupportedOperationException();
		}

		final Value[] rightType = new Value[] { instruction.operation.argumentType };

		String beforeRight = "", afterRight = "";
		if (hasRuntimeChecks && operation.checkDivisor) {
			beforeRight = checkDivisorFunction + "(";
			afterRight = ")";
		}

		final String right = beforeRight
				+ generateExpression(instruction.right, rightType) + afterRight
				+ ";";

		return generateMemoryAccess(instruction.left, rightType[0]) + " "
				+ operator + " " + right;
	}

	private String generateUnaryInstruction(UnaryInstruction instruction) {
		throw new UnsupportedOperationException();
	}

	private String generateExpression(Expression expression, final Value[] type) {
		class ExpressionVisitor implements IExpressionVisitor {
			public String str;

			@Override
			public void visit(BinaryExpression binaryExpression) {
				final BinaryOperation operation = binaryExpression.operation;
				final Value[] operandType = new Value[] { operation.operandType };

				if (type[0] == AnyValue.type) {
					type[0] = operandType[0];
				}

				final String operator = generateBinaryOperation(operation);

				String beforeRight = "", afterRight = "";
				if (hasRuntimeChecks && operation.checkDivisor) {
					beforeRight = checkDivisorFunction + "(";
					afterRight = ")";
				}

				str = generateExpression(binaryExpression.left, operandType)
						+ " "
						+ operator
						+ " "
						+ beforeRight
						+ generateExpression(binaryExpression.right,
								operandType) + afterRight;
			}

			@Override
			public void visit(ReadableExpression readableExpression) {
				str = generateReadable(readableExpression.value, type);
			}
		}

		final ExpressionVisitor visitor = new ExpressionVisitor();
		expression.accept(visitor);
		return visitor.str;
	}

	private String generateBinaryOperation(BinaryOperation operation) {
		final HashMap<BinaryOperation, String> operations = new HashMap<BinaryOperation, String>() {
			private static final long serialVersionUID = 1802915878572550172L;
			{
				put(BinaryOperation.S64add, "+");
				put(BinaryOperation.S64sub, "-");
				put(BinaryOperation.S64mul, "*");
				put(BinaryOperation.S64div, "/");
				put(BinaryOperation.S64mod, "%");
				put(BinaryOperation.S64and, "&");
				put(BinaryOperation.S64or, "|");
				put(BinaryOperation.S64xor, "^");
				put(BinaryOperation.F64add, "+");
				put(BinaryOperation.F64sub, "-");
				put(BinaryOperation.F64mul, "*");
				put(BinaryOperation.F64div, "/");
				// put(BinaryOperation.F64mod, "%");
			}
		};

		final String str = operations.get(operation);
		if (str == null) {
			throw new UnsupportedOperationException();
		}

		return str;
	}

	private String generateBooleanExpression(BooleanExpression expression) {
		final Value[] type = new Value[] { expression.operation.operandType };
		return generateReadable(expression.left, type) + " "
				+ generateBooleanOperation(expression.operation) + " "
				+ generateReadable(expression.right, type);
	}

	private String generateBooleanOperation(BooleanOperation operation) {
		final HashMap<BooleanOperation, String> operations = new HashMap<BooleanOperation, String>() {
			private static final long serialVersionUID = 779864823563124294L;
			{
				put(BooleanOperation.S64equal, "==");
				put(BooleanOperation.S64unequal, "!=");
				put(BooleanOperation.S64smaller, "<");
				put(BooleanOperation.S64smallerEqual, "<=");
				put(BooleanOperation.S64greater, ">");
				put(BooleanOperation.S64greaterEqual, ">=");
			}
		};

		final String str = operations.get(operation);
		if (str == null) {
			throw new UnsupportedOperationException();
		}

		return str;
	}

	private String generateReadable(Readable right, final Value[] type) {
		class ReadableVisitor implements IReadableVisitor {
			public String str;

			@Override
			public void visit(Literal readable) {
				class ValueVisitor implements IValueVisitor {
					public String str;

					@Override
					public void visited(IntegerValue value) {
						str = "" + value.value;
						if (type[0] == AnyValue.type) {
							type[0] = IntegerValue.type;
						}
					}

					@Override
					public void visited(DoubleValue value) {
						str = "" + value.value;
						if (type[0] == AnyValue.type) {
							type[0] = DoubleValue.type;
						}
					}

					@Override
					public void visited(AnyValue value) {
						str = memoryCellTypeName + "()"; // TODO: bessere
															// Lösung
						// finden
					}

					@Override
					public void visited(CharacterValue characterValue) {
						final char c = characterValue.value;

						if (c >= 30 && c <= 127) {
							str = "'" + escapeCharacter(c) + "'";
						} else {
							str = "" + (int) c;
						}

						if (type[0] == AnyValue.type) {
							type[0] = CharacterValue.type;
						}
					}

					private String escapeCharacter(char c) {
						switch (c) {
						case '\'':
							return "\\\'";
						case '\"':
							return "\\\"";
						case '\n':
							return "\\n";
						case '\r':
							return "\\r";
						case '\t':
							return "\\t";
						case '\\':
							return "\\\\";
						default:
							return "" + c;
						}
					}

					@Override
					public void visited(AddressValue addressValue) {
						str = getStaticAddressName(addressValue.address);
						if (type[0] == AnyValue.type) {
							type[0] = IntegerValue.type;
						}
					}
				}

				ValueVisitor visitor = new ValueVisitor();
				readable.value.visit(visitor);
				str = visitor.str;
			}

			@Override
			public void visit(ReadableFromAddress readable) {
				str = generateMemoryAccess(readable.location, type[0]);
			}

			@Override
			public void visit(AddressOf addressOf) {
				str = getStaticAddressName(addressOf.address);

				if (type[0] == AnyValue.type) {
					type[0] = IntegerValue.type;
				}
			}

			@Override
			public void visit(Key key) {
				str = stdInputName + "()";

				if (type[0] == AnyValue.type) {
					type[0] = IntegerValue.type;
				}
			}
		}

		ReadableVisitor visitor = new ReadableVisitor();
		right.accept(visitor);
		return visitor.str;
	}

	private String generateMemoryAccess(Address address, Value type) {
		class AddressVisitor implements IAddressVisitor {
			public String str;
			private final Value type;

			public AddressVisitor(Value type) {
				this.type = type;
			}

			@Override
			public void visited(IndirectAddress address) {
				str = memoryAccessFunction
						+ "("
						+ generateMemoryAccess(address.addressLocation,
								IntegerValue.type) + ")"
						+ generateValueAccessTypePostfix(type);
			}

			@Override
			public void visited(StaticAddress address) {
				str = memoryAccessFunction + "("
						+ getStaticAddressName(address.value) + ")"
						+ generateValueAccessTypePostfix(type);
			}
		}

		AddressVisitor visitor = new AddressVisitor(type);
		address.visit(visitor);
		return visitor.str;
	}

	public String getStaticAddressName(long address) {
		final String label = program.code.get((int) address).label;
		if (label != null) {
			return namedAddressPrefix + label;
		} else {
			return "" + address;
		}
	}

	private String generateValueAccessTypePostfix(Value type) {
		class ValueVisitor implements IValueVisitor {
			public String str;

			@Override
			public void visited(IntegerValue value) {
				str = "." + s64ValueFieldName;
			}

			@Override
			public void visited(DoubleValue value) {
				str = "." + f64ValueFieldName;
			}

			@Override
			public void visited(AnyValue value) {
				str = "";
			}

			@Override
			public void visited(CharacterValue characterValue) {
				str = "." + s64ValueFieldName;
			}

			@Override
			public void visited(AddressValue addressValue) {
				str = "." + s64ValueFieldName;
			}
		}

		ValueVisitor visitor = new ValueVisitor();
		type.visit(visitor);
		return visitor.str;
	}
}
