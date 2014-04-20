sha1 = require("sha1")
integer = require(sha1.create(12, 34, 56, ..))
array = require("array")
algorithm = require("algorithm")
range = require("range")

return package({
	description = "conversion between big endian byte sequences and integers"
	version = ::version(1, 0)
	export =
	{
		encode = (integer.u32 value) => {
			#value the number to be converted into big endian
			@return #value in big endian
			offsets = integer.range(integer.u32, 3, 0)
			return offsets.transform((offset) => value.shift_right(offset).truncate(byte))
		}

		encode = (integer.unsigned decoded) => (decoded value) => {
			#value the number to be converted into big endian
			@return #value in big endian
			offsets = integer.range(decoded, decoded.byte_size.sub(1), 0)
			return offsets.transform((offset) => value.shift_right(offset).truncate(byte))
		}

		decode = (array.array(byte, 4) bytes) => {
			#bytes bytes to be interpreted as big endian
			@return the number that is represented by #bytes
			offsets = integer.range(integer.u32, 3, 0)
			return algorithm.tie(bytes, offsets).accumulate(0, (sum, digit, offset) => (sum.or(digit.widen(integer.u32).shift_left(offset))))
		}
	}
})
