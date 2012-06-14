package taac.intermediate;

public abstract class Initializer {
	public abstract long getSize();

	public abstract void accept(IInitializerVisitor visitor);
}
