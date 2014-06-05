filesystem = require("filesystem", 1.0)
tcp = require("tcp", 1.0)
http = require("http", 1.0)

serve = (source(tcp.client) clients, filesystem.directory static_files) {
	return clients.map((tcp.client client) => {
		request = http.parse_request(client.incoming)
		positive_response = regex.match_first(
			request.method,
			[
				(regex.from_literal("GET")) => {
					requested_file = static_files.read_file(filesystem.path.from_string(request.path))
					return match(requested_file, [
						(filesystem.error) => http.make_response(http.file_not_found),
						(source(byte) content) => http.deliver_file(http.ok, content)
					])
				}
			]
		)
		response = positive_response.value_or(http.make_response(http.method_not_allowed))
		return client.outgoing.write(response)
	})
}
