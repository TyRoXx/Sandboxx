package taac.assembly;

import taac.intermediate.AddressValue;
import taac.intermediate.ValueInitializer;

public class AddressOfInitializer extends Initializer {
	private final String name;

	public AddressOfInitializer(String name) {
		this.name = name;
	}

	@Override
	public long getSize() {
		return 1;
	}

	@Override
	public taac.intermediate.Initializer translate(ParserOutput parsed)
			throws SemanticException {
		return new ValueInitializer(new AddressValue(parsed.getAddressOf(name)));
	}
}
