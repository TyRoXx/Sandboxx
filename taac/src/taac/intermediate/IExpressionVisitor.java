package taac.intermediate;

public interface IExpressionVisitor {
	void visit(BinaryExpression binaryExpression);

	void visit(ReadableExpression readableExpression);
}
