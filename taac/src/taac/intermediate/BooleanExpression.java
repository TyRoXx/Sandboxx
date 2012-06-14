package taac.intermediate;

public class BooleanExpression {
	public final Readable left, right;
	public final BooleanOperation operation;
	
	public BooleanExpression(Readable left, Readable right, BooleanOperation operation) {
		this.left = left;
		this.right = right;
		this.operation = operation;
	}
}
