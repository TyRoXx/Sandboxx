#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <array>
#include <climits>
#include <fstream>
#include <cstdlib>

#include <netinet/in.h>

struct Object
{
	std::uint32_t i;
};


template <class Integer>
void encode_big_endian_wrong(std::ostream &out, Integer value)
{
	switch (sizeof(value))
	{
	case 1: break;
	case 2: value = htons(value); break;
	case 4: value = htonl(value); break;
	default: throw std::invalid_argument("I am too stupid to support arbitrary integer sizes");
	}
	out.write(reinterpret_cast<char *>(&value), sizeof(value));
}


//value muss unsigned sein, weil bei negativen Werten das Shiften undefiniertes Verhalten hätte.
template <class Unsigned>
typename std::enable_if<std::is_unsigned<Unsigned>::value, void>::type
encode_big_endian_correct_impl(std::ostream &out, Unsigned value)
{
	//Der Puffer soll die Anzahl der Aufrufe von out.write verringern, damit das nicht langsamer
	//wird als die falsche Variante.
	std::array<unsigned char, sizeof(value)> buffer;
	for (std::size_t i = 0; i < buffer.size(); ++i)
	{
		buffer[i] = static_cast<unsigned char>(value >> ((buffer.size() - i - 1) * CHAR_BIT));
	}
	out.write(reinterpret_cast<char *>(buffer.data()), buffer.size());
}

template <class Integer>
void encode_big_endian_correct(std::ostream &out, Integer value)
{
	encode_big_endian_correct_impl(out, static_cast<typename std::make_unsigned<Integer>::type>(value));
}

int main()
{
	std::ofstream file("out.bin", std::ios::binary);
	if (!file)
	{
		return 1;
	}
	Object o;
	o.i = std::rand();
	encode_big_endian_correct(file, o.i);
	encode_big_endian_wrong(file, o.i);

	//Beide "funktionieren", aber nur eine Variante
	//ist portabel und funktioniert für alle Integer-Größen.
}
