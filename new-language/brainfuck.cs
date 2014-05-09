array = require("array", 1.0)
vector = require("vector", 1.0)

return {
	size_dependent = (uint program_size, uint memory_size) => {
		state = {
			memory = array(byte, memory_size)
			memory_ptr = uint.limited(0, memory_size.sub(1))
			program_ptr = uint.limited(0, program_size)
		}

		next_instruction = (state.program_ptr current) {
			return current.mod_add(1)
		}

		move = (state previously, permutation(state.memory_ptr) advance) {
			return state {
				memory = previously.memory
				memory_ptr = advance(previously.memory_ptr)
				program_ptr = next_instruction(previously.program_ptr)
			}
		}

		right = (state previously) => move(previously, state.memory_ptr.mod_sub(1))
		left = (state previously) => move(previously, state.memory_ptr.mod_add(1))

		change = (state previously, permutation(byte) operation) {
			return state {
				memory = previously.memory.transform_element(previously.memory_ptr, operation)
				memory_ptr = previously.memory_ptr
				program_ptr = next_instruction(previously.program_ptr)
			}
		}

		increment = (state previously) => change(previously, byte.mod_add)
		decrement = (state previously) => change(previously, byte.mod_sub)

		jump_if = (state previously, state.program_ptr destination) {
			return state {
				memory = previously.memory
				memory_ptr = previously.memory_ptr
				program_ptr = if(previously.memory.get(previously.memory_ptr).equals(0), destination, next_instruction(previously.program_ptr))
			}
		}

		jump = (state previously, state.program_ptr destination) {
			return state {
				memory = previously.memory
				memory_ptr = previously.memory_ptr
				program_ptr = destination
			}
		}

		write = (state previously, IO(consumer(byte)) write_byte) state {
			write_byte(previously.memory.get(previously.memory_ptr))
			return state {
				memory = previously.memory
				memory_ptr = previously.memory_ptr
				program_ptr = next_instruction(previously.program_ptr)
			}
		}

		read = (state previously, IO(producer(byte)) read_byte) state {
			value = read_byte()
			return state {
				memory = previously.memory.set(previously.memory_ptr, value)
				memory_ptr = previously.memory_ptr
				program_ptr = next_instruction(previously.program_ptr)
			}
		}

		environment = {
			write = IO(consumer(byte))
			read = IO(producer(byte))
		}

		uniform_instruction = {
			left      = (state previously, environment env) => left(previously)
			right     = (state previously, environment env) => right(previously)
			increment = (state previously, environment env) => increment(previously)
			decrement = (state previously, environment env) => decrement(previously)
			write     = (state previously, environment env) => write(previously, env.write)
			read      = (state previously, environment env) => read(previously, env.read)
			jump      = (state previously, environment env, state.program_ptr destination) => jump(previously, destination)
			jump_if   = (state previously, environment env, state.program_ptr destination) => jump_if(previously, destination)
		}

		symbol = {
			"left"
			"right"
			"increment"
			"decrement"
			"write"
			"read"
			"enter_loop"
			"leave_loop"
		}

		parse_one = (variant(symbol) source, producer(state.program_ptr) find_matching_leave_loop, producer(state.program_ptr) find_matching_enter_loop) => tail_bound(uniform_instruction, 2) {
			return source.visit({
				"left" = () => uniform_instruction.left
				"right" = () => uniform_instruction.right
				"increment" = () => uniform_instruction.increment
				"decrement" = () => uniform_instruction.decrement
				"write" = () => uniform_instruction.write
				"read" = () => uniform_instruction.read
				"enter_loop" = () => (state previously, environment env) => uniform_instruction.jump_if(previously, env, find_matching_leave_loop().mod_add(1))
				"leave_loop" = () => (state previously, environment env) => uniform_instruction.jump(previously, env, find_matching_enter_loop())
			})
		}

		parse = (array(variant(symbol), program_size) source) => array(tail_bound(uniform_instruction, 2), program_size) {
			return source.iterate((state.program_ptr index) => {
				find_matching_leave_loop = TODO
				find_matching_enter_loop = TODO
				return parse_one(source.get(index), find_matching_leave_loop, find_matching_enter_loop)
			})
		}
	}
}
