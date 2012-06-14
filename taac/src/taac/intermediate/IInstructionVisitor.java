package taac.intermediate;

public interface IInstructionVisitor {
	void visit(AssignmentInstruction binary);

	void visit(Jump jump);

	void visit(ConditionalJump conditionalJump);

	void visit(UnaryInstruction unaryInstruction);

	void visit(Stop stop);

	void visit(NonExecutable nonExecutable);

	void visit(Print print);

	void visit(Push push);

	void visit(Pull pull);

	void visit(GetPull getPull);
}
