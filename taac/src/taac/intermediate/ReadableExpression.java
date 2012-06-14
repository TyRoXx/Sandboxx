package taac.intermediate;

public class ReadableExpression extends Expression {
	public final Readable value;

	public ReadableExpression(Readable value) {
		this.value = value;
	}

	@Override
	public void accept(IExpressionVisitor visitor) {
		visitor.visit(this);
	}
}
