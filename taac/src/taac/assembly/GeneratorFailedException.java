package taac.assembly;

public class GeneratorFailedException extends Exception {
	private static final long serialVersionUID = -2018839602132629310L;
	
	public final int errorCount;
	
	public GeneratorFailedException(int errorCount) {
		super("Parser failed with " + errorCount + " errors");
		
		this.errorCount = errorCount;
	}
}
