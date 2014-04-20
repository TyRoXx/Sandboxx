tcp = require("tcp")

return package({
	export = {
		main = (tcp.stack stack) => {
			return stack.create_acceptor(tcp.port(12345),
				(tcp.acceptor acceptor) => {
					return acceptor.accept((tcp.client client) => {
						sender = client.buffered_sender()
						return client.buffered_receiver((byte b) => {
							return sender(b)
						})
					})
				})
		}
	}
})
