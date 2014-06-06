filesystem = require("filesystem", version(1))
tcp = require("tcp", version(1))
http = require("http", version(1))

make_file_server = (filesystem.directory static_files) {
	get = (http.request request) {
		requested_file = static_files.read_file(filesystem.path.from_string(request.path))
		responses = tuple.empty
			.append((filesystem.error) => http.make_response(http.file_not_found))
			.append((source(byte) content) => http.deliver_file(http.ok, content))
		return requested_file.match(responses)
	}
	serve_one = (tcp.client client) {
		request = http.parse_request(client.incoming)
		methods = map.empty.insert("GET", () => get(request, static_files))
		maybe_respond = methods.find(request.method)
		respond = maybe_respond.value_or(() => http.make_response(http.method_not_allowed))
		response = respond()
		return client.outgoing.write(response)
	}
	return serve_one
}

serve_all = (source(tcp.client) clients, filesystem.directory static_files) {
	serve = make_file_server(static_files)
	return clients.map(serve)
}

return create_package(
	version(1),
	map.empty
		.insert("make_file_server", make_file_server)
		.insert("serve_all", serve_all)
)
