package taac.assembly;

import taac.intermediate.ReadableFromAddress;

public class ReadFromAddress extends Readable {
	public final Address access;

	public ReadFromAddress(Address access) {
		this.access = access;
	}

	@Override
	public taac.intermediate.Readable translate(ParserOutput parsed)
			throws SemanticException {
		return new ReadableFromAddress(access.translate(parsed));
	}
}
