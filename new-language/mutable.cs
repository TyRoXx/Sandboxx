
main = (source(tcp.client) clients) {
	visitor_counter = make_mutable(uint64(0))
	spawn(() => one_of_each(clients, visitor_counter).for_each((tcp.client client, uint64 past_visitors) {
		visitor_counter.assign(past_visitors.add(1))
		spawn(() => {
			request = http.parse_request(client.incoming)
			response = http.make_response([
				status = http.ok
				content = text.format("You are visitor {{decimal}}", past_visitors.add(1))
			])
			response.write(client.outgoing)
		})
	}))
}
