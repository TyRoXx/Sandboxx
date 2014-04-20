tcp = require("tcp")
async = require("async")

serve_client = () async => {
}

return package({
	export = {
		main = (tcp.stack stack, async.spawner spawn) async => {
			tcp.acceptor acceptor = await stack.create_acceptor(tcp.port(12347))
			tcp.incoming_clients clients = acceptor.incoming_clients
			return async.loop(() async => {
				tcp.client client = await clients.next
				spawn(() async => serve_client())
			})
		}
	}
})
