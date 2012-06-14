package taac.assembly;

public class CompilerFailedException extends Exception {
	private static final long serialVersionUID = 4284362583000289446L;
	
	public final int errorCount;
	
	public CompilerFailedException(int errorCount)
	{
		super("Compiler failed with " + errorCount + " errors");
		
		this.errorCount = errorCount;
	}
}
