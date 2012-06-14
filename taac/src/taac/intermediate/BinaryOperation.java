package taac.intermediate;

public enum BinaryOperation {
	S64add(IntegerValue.type, false),
	S64sub(IntegerValue.type, false),
	S64mul(IntegerValue.type, false),
	S64div(IntegerValue.type, true),
	S64mod(IntegerValue.type, true),
	S64and(IntegerValue.type, false),
	S64or(IntegerValue.type, false),
	S64xor(IntegerValue.type, false),
	F64add(DoubleValue.type, false),
	F64sub(DoubleValue.type, false),
	F64mul(DoubleValue.type, false),
	F64div(DoubleValue.type, false),
	F64mod(DoubleValue.type, false),
	F64pow(DoubleValue.type, false),
	;

	public final Value operandType;
	public final boolean checkDivisor;

	private BinaryOperation(Value operandType, boolean checkDivisor) {
		this.operandType = operandType;
		this.checkDivisor = checkDivisor;
	}
}
