extern crate hyper;

use std::io::net::ip::Ipv4Addr;
use hyper::header::ContentLength;
use hyper::server::{Server, Request, Response};
use hyper::uri::RequestUri::AbsolutePath;

fn respond(req: Request, mut res: Response) {
	match req.uri {
		AbsolutePath(_) => {
			let message = b"Hello";
			res.headers_mut().set(ContentLength(message.len() as u64));
			let mut res = res.start().unwrap();
			res.write(message).unwrap();
			res.end().unwrap();
		},
		_ => {
		}
	}
}

fn main() {
	let server = Server::http(Ipv4Addr(127, 0, 0, 1), 1337);
	let mut listening = server.listen(respond).unwrap();
	listening.await();
}
