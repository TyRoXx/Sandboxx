map = require("map")
partial = require("partial")

variant = (type types...) => {
	operator[] = overload(types.accumulate(set.empty, (set(signature) sum, type element) => sum.insert(signature(element, meta(element)))))
}

return package({
	export = {
		variant = ::variant
		create = (type types...) => (type value) => variant(types...) {
			return 
		}
	}
})
