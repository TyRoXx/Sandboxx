make_lexer = (type source_character)
	token_kind = enum.empty
		.append("newline")
		.append("space")
		.append("tab")
		.append("left_parenthesis")
		.append("right_parenthesis")
		.append("dot")
		.append("comma")
		.append("identifier")
		.append("integer")

	make_token = (token_kind kind, range(source_character) position)
		return map.empty
			.insert("position", position)
			.insert("kind", kind)

	read_token = (range(source_character) source)
		matched = regex.match(source,
			map.empty
				.append(regex.literal("\n"), (range(source_character) position) => make_token(token_kind("newline"), position))
				.append(regex.literal(" "), (range(source_character) position) => make_token(token_kind("space"), position))
				.append(regex.literal("\t"), (range(source_character) position) => make_token(token_kind("tab"), position))
				.append(regex.literal("("), (range(source_character) position) => make_token(token_kind("left_parenthesis"), position))
				.append(regex.literal(")"), (range(source_character) position) => make_token(token_kind("right_parenthesis"), position))
				.append(regex.literal("."), (range(source_character) position) => make_token(token_kind("dot"), position))
				.append(regex.literal(","), (range(source_character) position) => make_token(token_kind("comma"), position))
				.append(regex.range('a', 'z').both_cases().or('_').minimum(1), (range(source_character) position) => make_token(token_kind("identifier"), position))
				.append(regex.range('0', '9').minimum(1), (range(source_character) position) => make_token(token_kind("integer"), position))
		)
		return map.empty
			.insert("token", matched.result)
			.insert("rest", matched.rest)

return create_package(
	version(1),
	map.empty
		.insert("make_lexer", make_lexer)
)
