package taac.assembly;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class ParserOutput {
	public final Map<String, Long> addressesByName = new HashMap<String, Long>();
	public final Map<Long, Initializer> initializers = new HashMap<Long, Initializer>();
	public final ArrayList<StatementLine> statements = new ArrayList<StatementLine>();
	public boolean isKeyDefined = false;
	public boolean hasStack = false;
	public long stackPointerAddress = 0;

	public long getAddressOf(String label) throws UnknownIdentifierException {
		final Long address = addressesByName.get(label);
		if (address == null) {
			throw new UnknownIdentifierException(label);
		}
		return address;
	}
}
