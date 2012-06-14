package taac.assembly;

public class AddressOf extends Readable {
	private final String name;

	public AddressOf(String name) {
		this.name = name;
	}

	@Override
	public taac.intermediate.Readable translate(ParserOutput parsed)
			throws SemanticException {
		return new taac.intermediate.AddressOf(parsed.getAddressOf(name));
	}
}
