package taac.assembly;

public class ParsedVariable {
	public final String name;
	public final long line;

	public ParsedVariable(String name, long line) {
		this.name = name;
		this.line = line;
	}

	@Override
	public String toString() {
		String str = name + "(line " + line + ")";
		return str;
	}
}
