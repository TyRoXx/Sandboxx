package taac.intermediate;

public enum AssignmentOperation {
	A64copy(AnyValue.type, false),

	S64copy(IntegerValue.type, false),
	S64add(IntegerValue.type, false),
	S64sub(IntegerValue.type, false),
	S64mul(IntegerValue.type, false),
	S64div(IntegerValue.type, true),
	S64mod(IntegerValue.type, true),
	S64and(IntegerValue.type, false),
	S64or(IntegerValue.type, false),
	S64xor(IntegerValue.type, false),
	S64shiftLeft(IntegerValue.type, false),
	S64shiftRight(IntegerValue.type, false),
	U64shiftRight(IntegerValue.type, false),

	F64copy(DoubleValue.type, false),
	F64add(DoubleValue.type, false),
	F64sub(DoubleValue.type, false),
	F64mul(DoubleValue.type, false),
	F64div(DoubleValue.type, false),
	F64mod(DoubleValue.type, false),
	;
	
	public final Value argumentType;
	public final boolean checkDivisor;
	
	private AssignmentOperation(Value argumentType, boolean checkDivisor){
		this.argumentType = argumentType;
		this.checkDivisor = checkDivisor;
	}
}
