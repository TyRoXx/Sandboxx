tcp = require("tcp")
signal = require("signal")
unicode = require("unicode")
io = require("io")
utf8 = require("utf8")
vector = require("vector")

return package({
	export = {
		main = (tcp.stack stack) => {
			return stack.create_acceptor(tcp.port(12346),
				(tcp.acceptor acceptor) => {
					message_received = signal.create(unicode.string)
					return acceptor.accept((tcp.client client) => {
						sender = client.buffered_sender()
						receiver_buffer = io.mutable(vector(byte))
						receiver = client.buffered_receiver((byte b) => {
							if (b == 10) {
								message = utf8.decode(receive_buffer.value)
								return io.any(
									receive_buffer.assign(vector(byte).empty),
									message_received.emit(message))
							}
							return receive_buffer.assign(receive_buffer.value.push_back(b))
						})
						return io.any(
							message_received.connect((unicode.string message) => {
								return sender(utf8.encode(message))
							}),
							receiver)
					})
				})
		}
	}
})
