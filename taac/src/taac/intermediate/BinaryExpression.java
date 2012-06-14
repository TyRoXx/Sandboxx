package taac.intermediate;

public class BinaryExpression extends Expression {
	public final Expression left, right;
	public final BinaryOperation operation;

	public BinaryExpression(Expression left, Expression right,
			BinaryOperation operation) {
		this.left = left;
		this.right = right;
		this.operation = operation;
	}

	@Override
	public void accept(IExpressionVisitor visitor) {
		visitor.visit(this);
	}
}
