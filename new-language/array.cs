memory = require("memory")
partial = require("partial")
test = require("test")

array_index = (memory.size length) => partial.with_predicate(memory.size, (memory.size n) => (n < length))

array = (type contained, memory.size length) => return {
	operator[] = signature(contained, array_index(length))
}

return package({
	description = "a fixed-length sequence of equally typed elements"
	version = ::version(1, 0)
	export = {	
		index = array_index
		array = ::array
		create = return overload(
			(auto head) => array(typeof(head), 1) {
				operator[] = (array_index(1) index) => {
					return head
				}
			},
			(auto head, typeof(head) second, ...) => array(typeof(head), sizeof(...).add(2)) {
				operator[] = (array_index(sizeof(...).add(2)) index) => {
					if (index.equals(0)) {
						return head
					}
					return create(second, ...)[index.sub(1)]
				}
			}
		)
		fill = (type contained, memory.size length) => (contained element) => array(contained, length) {
			operator[] = (array_index(length) index) => {
				return element
			}
		}
	}
})
