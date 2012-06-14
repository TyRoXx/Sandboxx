package taac.intermediate;

public interface IInitializerVisitor {
	void visit(StringInitializer stringInitializer);

	void visit(ValueInitializer valueInitializer);
}
