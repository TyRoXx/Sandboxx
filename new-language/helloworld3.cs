unicode = require("unicode", 1.0)
io = require("io", 1.0)

return {
	main = (IO(consumer(unicode.char)) output) IO() {
		io.copy("Hello, world!\n".begin(), output)
	}
}
