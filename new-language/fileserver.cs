tcp = require("tcp")
unicode = require("unicode")
io = require("io")
http = require("http-server")
fs = require("filesystem")

answer_get = (io.byte_writer sender, fs.directory served_dir, http.path requested) async => {
	fs.file_reader file = await served_dir.read_file(fs.path(request.path.utf8()))
	await sender(http.respond_ok(file.length))
	await sender(file.content)
}

answer_get2 = (io.byte_writer sender, fs.directory served_dir, http.path requested) => {
	return served_dir.read_file(fs.path(request.path.utf8())).then((fs.file_reader file) => {
		return sender(http.respond_ok(file.length)).then(() => {
			return sender(file.content)
		})
	})
}

answer_request = (io.byte_writer sender, fs.directory served_dir, http.request request) async => {
	if (request.method.to_upper().equals("GET")) {
		await answer_get(sender, served_dir, request.path)
	}
	else {
		await sender(http.respond_not_supported())
	}
}

serve_client = (tcp.client client, fs.directory served_dir) async => {
	http.request request = await http.parse_request(io.buffered_reader(client.receive))
	await answer_request(io.buffered_writer(client.send), served_dir, request)
	await client.shutdown()
}

return package({
	export = {
		main = (tcp.stack stack, fs.directory served_dir, io.spawner spawn) IO => {
			tcp.acceptor acceptor = stack.create_acceptor(tcp.port(8080))
			io.loop(() IO => {
				tcp.client client = await acceptor.accept()
				spawn(() async => serve_client(client, served_dir))
			})
		}
	}
})
