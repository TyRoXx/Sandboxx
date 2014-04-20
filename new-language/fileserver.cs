tcp = require("tcp")
unicode = require("unicode")
io = require("io")
http = require("http-server")
fs = require("filesystem")

answer_get = (io.byte_writer sender, fs.directory served_dir, http.path requested) IO => {
	fs.file_reader file = served_dir.read_file(fs.path(request.path.utf8()))
	sender(http.respond_ok(file.length))
	sender(file.content)
}

answer_request = (io.byte_writer sender, fs.directory served_dir, http.request request) IO => {
	if (request.method.to_upper().equals("GET")) {
		answer_get(sender, served_dir, request.path)
	}
	else {
		sender(http.respond_not_supported())
	}
}

serve_client = (tcp.client client, fs.directory served_dir) IO => {
	http.request request = http.parse_request(io.buffered_reader(client.receive))
	answer_request(io.buffered_writer(client.send), served_dir, request)
	client.shutdown()
}

return package({
	export = {
		main = (tcp.stack stack, fs.directory served_dir, io.spawner spawn) IO => {
			tcp.acceptor acceptor = stack.create_acceptor(tcp.port(8080))
			io.loop(() IO => {
				tcp.client client = acceptor.accept()
				spawn(() IO => serve_client(client, served_dir))
			})
		}
	}
})
