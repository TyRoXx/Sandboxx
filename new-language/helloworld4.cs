utf8 = require("utf8", 1.0)
unicode = require("unicode", 1.0)
io = require("io", 1.0)

return {
	main = (IO(consumer(byte)) output) IO() {
		producer(byte) message = utf8.encode("Hällo, wörld!\n")
		io.copy(message, output)
	}
}
