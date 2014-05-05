array = require("array", 1.0)
vector = require("vector", 1.0)

return {
	size_dependent = (uint program_size, uint memory_size) => {
		state = {
			memory = array(byte, memory_size)
			memory_ptr = uint.limited(0, memory_size.sub(1))
			program_ptr = uint.limited(0, program_size)
		}

		move_direction = {
			left = state.memory_ptr.mod_sub(1)
			right = state.memory_ptr.mod_add(1)
		}

		move = (state previously, move_direction dir) {
			return state {
				memory = previously.memory
				memory_ptr = dir(previously)
				program_ptr = previously.program_ptr.mod_add(1)
			}
		}

		right = (state previously) => move(previously, move_direction.right)
		left = (state previously) => move(previously, move_direction.left)

		change_operation = {
			increment = byte.mod_add
			decrement = byte.mod_sub
		}

		change = (state previously, change_operation operation) {
			return state {
				memory = previously.memory.transform_element(previously.memory_ptr, operation)
				memory_ptr = previously.memory_ptr
				program_ptr = previously.program_ptr.mod_add(1)
			}
		}

		increment = (state previously) => change(previously, change_operation.increment)
		decrement = (state previously) => change(previously, change_operation.decrement)

		parse = {
			right = right
			left = left
			increment = increment
			decrement = decrement
			write
			read
			begin_loop
			end_loop
		}
	}
}
