package taac.assembly;

public class ParserFailedException extends Exception {
	private static final long serialVersionUID = -5896156379968312250L;
	
	public final int errorCount;
	
	public ParserFailedException(int errorCount)
	{
		super("Parser failed with " + errorCount + " errors");
		
		this.errorCount = errorCount;
	}
}
