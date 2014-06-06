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
				.insert(regex.literal("\n"), token_kind("newline"))
				.insert(regex.literal(" "), token_kind("space"))
				.insert(regex.literal("\t"), token_kind("tab"))
				.insert(regex.literal("("), token_kind("left_parenthesis"))
				.insert(regex.literal(")"), token_kind("right_parenthesis"))
				.insert(regex.literal("."), token_kind("dot"))
				.insert(regex.literal(","), token_kind("comma"))
				.insert(regex.range('a', 'z').both_cases().or('_').minimum(1), token_kind("identifier"))
				.insert(regex.range('0', '9').minimum(1), token_kind("integer"))
		)
		return map.empty
			.insert("token", make_token(matched.result, matched.position))
			.insert("rest", matched.rest)

return create_package(
	version(1),
	map.empty
		.insert("make_lexer", make_lexer)
)
