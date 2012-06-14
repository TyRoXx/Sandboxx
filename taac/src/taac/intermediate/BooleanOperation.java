package taac.intermediate;

public enum BooleanOperation {
	S64equal(IntegerValue.type),
	S64unequal(IntegerValue.type),
	S64smaller(IntegerValue.type),
	S64smallerEqual(IntegerValue.type),
	S64greater(IntegerValue.type),
	S64greaterEqual(IntegerValue.type),
	;
	
	public final Value operandType;

	private BooleanOperation(Value operandType) {
		this.operandType = operandType;
	}
}
