typeof = (type T value)
	return T

make_variant = (set(type) Ts)
	return (Ts E element)
		return {
			element_type = () type
				return E

			try_get = (Ts got) -> optional(got)
				return got.equals(E).branch(
					() => some(element),
					() => none(E))
		}

variant = (set(type) Ts)
	return common_interface(Ts.fmap((type T)
		return (T value) => make_variant(Ts)(value)))

nothing = {}

options = (type T)
	return empty_set.insert(T).insert(nothing)

some = (type T value)
	return make_variant(options(T))(value)

none = (type T)
	return make_variant(options(T))(nothing)

optional = (type T)
	return common_interface({
		(T value) => some(value),
		() => none(T)})
