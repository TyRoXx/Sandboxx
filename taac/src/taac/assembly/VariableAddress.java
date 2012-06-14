package taac.assembly;

public class VariableAddress extends Address {
	private final String name;

	public VariableAddress(String name) {
		this.name = name;
	}

	@Override
	public taac.intermediate.Address translate(ParserOutput parsed)
			throws SemanticException {
		return new taac.intermediate.StaticAddress(parsed.getAddressOf(name));
	}
}
