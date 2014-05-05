
is_odd = (int64 n) {
	return n.mod(2).equal(1)
}

transmogrify = (int64 n) {
	return if(is_odd(n), n.add(1), n.sub(1))
}

return module({
	export = {
		main = (IO(producer(int64)) numbers) {
			return numbers.wrap(transmogrify)
		}
	}
})
