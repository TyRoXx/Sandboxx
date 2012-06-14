package taac.intermediate;

import java.util.ArrayList;
import java.util.Map;

public class IntermediateProgram {
	public final ArrayList<Statement> code;
	public final Map<Long, Initializer> initialMemoryValues;
	public final boolean hasStack;
	public final long stackPointerAddress;

	public IntermediateProgram(ArrayList<Statement> code,
			Map<Long, Initializer> initialMemoryValues, boolean hasStack, long stackPointerAddress) {
		this.code = code;
		this.initialMemoryValues = initialMemoryValues;
		this.hasStack = hasStack;
		this.stackPointerAddress = stackPointerAddress;
	}
}
