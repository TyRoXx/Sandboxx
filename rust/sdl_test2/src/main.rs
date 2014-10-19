extern crate sdl2;
use std::collections::{Deque, RingBuf};
use std::iter::count;
use sdl2::rect::{Rect, Point};

fn distance_squared(first: &Point, second: &Point) -> i32 {
	let x = first.x - second.x;
	let y = first.y - second.y;
	return (x * x) + (y * y);
}

fn main() {
	sdl2::init(sdl2::INIT_VIDEO);
	let window = sdl2::video::Window::new(
		"SDL2",
		sdl2::video::PosCentered,
		sdl2::video::PosCentered,
		1024,
		768,
		sdl2::video::OPENGL).unwrap();
	let renderer = sdl2::render::Renderer::from_window(
		window,
		sdl2::render::DriverAuto,
		sdl2::render::ACCELERATED).unwrap();
	let mut mouse_positions = RingBuf::new();
	'event_loop: loop {
		let _ = renderer.set_draw_color(sdl2::pixels::RGB(0, 0, 0));
		let _ = renderer.clear();
		let max_saved_steps = 255u8;
		loop {
			match sdl2::event::poll_event() {
				sdl2::event::QuitEvent(_) => break 'event_loop,
				sdl2::event::MouseMotionEvent(_,_,_,_,x,y,_,_) => {
					let new_pos = Point{x: x as i32, y: y as i32};
					loop {
						match mouse_positions.back() {
							Some(&last_pos) => {
								let min_distance = 10i32;
								if distance_squared(&new_pos, &last_pos) < (min_distance * min_distance) {
									break;
								}
							}
							None => {}
						}
						mouse_positions.push(new_pos);
						if mouse_positions.len() > (max_saved_steps as uint) {
							mouse_positions.pop_front();
						}
						break;
					}
				},
				sdl2::event::NoEvent => break,
				sdl2::event::KeyUpEvent(_,_,sdl2::keycode::EscapeKey,_,_) => break 'event_loop,
				_ => {}
			}
		}
		for (mouse_pos, color) in mouse_positions.iter().zip(count(0u8, 1u8)) {
			let _ = renderer.set_draw_color(sdl2::pixels::RGB(color, 255 - color, 127));
			let _ = renderer.draw_rect(&Rect::new(mouse_pos.x, mouse_pos.y, 10, 10));
		}
		renderer.present();
		sdl2::timer::delay(16);
	}
	sdl2::quit();
}
