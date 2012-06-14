package taac.intermediate;

public interface IAddressVisitor {
	void visited(IndirectAddress address);

	void visited(StaticAddress address);
}
