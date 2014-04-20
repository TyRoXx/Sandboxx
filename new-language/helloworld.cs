stdio = require("stdio")

return package({
	export = {
		main = (stdio.text_writer out) => {
			return out.write("Hello, world!\n")
		}
	}
})
