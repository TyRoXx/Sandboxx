unicode = require("unicode", 1.0)
test = require("test", 0.2)
random_access = require("random_access", 1.0)
char = unicode.char

exported = {
	main = () generator(char) {
		return "Hello, world!\n".begin()
	}

	tests = {
		"main works" = (IO(test.assert) assert) {
			random_access(char) expected = "Hello, world!\n"
			random_access(char) got = main().all()
			assert.equal(expected, got)
		}
	}
}
return exported
