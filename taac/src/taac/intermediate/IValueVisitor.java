package taac.intermediate;

public interface IValueVisitor {
	void visited(IntegerValue value);

	void visited(DoubleValue value);

	void visited(AnyValue value);

	void visited(CharacterValue characterValue);

	void visited(AddressValue addressValue);
}
