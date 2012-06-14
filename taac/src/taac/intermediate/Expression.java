package taac.intermediate;

public abstract class Expression {
	public abstract void accept(IExpressionVisitor visitor);
}
