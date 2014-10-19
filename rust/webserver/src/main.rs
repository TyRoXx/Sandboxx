extern crate hyper;

use std::io::net::ip::Ipv4Addr;

fn respond(mut incoming: hyper::server::Incoming) {
	for (mut request, mut response) in incoming {
		match request.uri {
			hyper::uri::AbsolutePath(ref path) => match (&request.method, path.as_slice()) {
				(&hyper::Get, "/") => {
					let out = b"Hello, world";
					response.headers_mut().set(hyper::header::common::ContentLength(out.len()));
					let mut content = response.start().unwrap();
					content.write(out).unwrap();
					content.end().unwrap();
				},
				_ => {
					*response.status_mut() = hyper::status::NotFound;
					response.start().unwrap().end();
				}
			},
			_ => {
				*response.status_mut() = hyper::status::NotFound;
				response.start().unwrap().end();
			}
		}
	}
}

fn main() {
	hyper::server::Server::http(Ipv4Addr(127,0,0,1), 8080).listen(respond).unwrap();
}
