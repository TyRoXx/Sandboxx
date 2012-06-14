package taac.backends;

public class Indentation {
	public final int width;

	private static final char character = '\t';

	public Indentation() {
		width = 0;
	}

	private Indentation(int width) {
		this.width = width;
	}

	public Indentation more() {
		return new Indentation(width + 1);
	}

	@Override
	public String toString() {
		StringBuilder str = new StringBuilder();
		for (int i = 0; i < width; ++i) {
			str.append(character);
		}
		return str.toString();
	}
}
