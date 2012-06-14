package taac.assembly;

public class IndirectAddress extends Address {
	public final Address address;

	public IndirectAddress(Address address) {
		this.address = address;
	}

	@Override
	public taac.intermediate.Address translate(ParserOutput parsed)
			throws SemanticException {
		return new taac.intermediate.IndirectAddress(address.translate(parsed));
	}
}
