package taac.intermediate;

public class CharacterValue extends Value {
	public final char value;

	public CharacterValue(char value) {
		this.value = value;
	}

	@Override
	public String toString() {
		return "'" + (char) value + "'";
	}

	@Override
	public void visit(IValueVisitor visitor) {
		visitor.visited(this);
	}

	public static final CharacterValue type = new CharacterValue('\0');
}
