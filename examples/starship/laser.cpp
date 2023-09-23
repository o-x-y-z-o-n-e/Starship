#include "laser.hpp"
#include "game.hpp"

laser::table laser::pool;

static void apply_damage(enemy& e) {
	e.take_damage();
}

laser::laser() {
	transform = rge::transform::create();
	sprite = rge::sprite::create();
	sprite->transform->parent = transform;
	sprite->pixels_per_unit = PPU;
	sprite->texture = rge::texture::load("res/laser.png");
}

void laser::reset() {

}

void laser::update(float delta_time) {
	const float SPEED = 20.0F;

	transform->position += rge::vec2(0, SPEED) * delta_time;
	rge::vec2 p = transform->position + rge::vec2(0, sprite->texture->get_height() / float(PPU));

	if(transform->position.y > 6) {
		destroy(this);
	}

	if(enemy::overlap_all(p, apply_damage)) {
		destroy(this);
	}
}

void laser::draw() {
	transform->position.z = LAYER_TO_Z(FX_LAYER);
	rge::engine::get_renderer()->draw(*sprite);
}

void laser::set_position(rge::vec2 p) {
	transform->position = p;
}

laser* laser::create() {
	table::iterator it;
	for(it = pool.begin(); it != pool.end(); it++) {
		if(!it->second) {
			it->second = true;
			it->first->reset();
			return it->first;
		}
	}

	laser* ins = new laser();
	pool.push_back(entry(ins, true));
	return ins;
}

void laser::destroy(laser* ins) {
	table::iterator it;
	for(it = pool.begin(); it != pool.end(); it++) {
		if(it->first == ins) {
			it->second = false;
			return;
		}
	}
}

void laser::destroy_all() {
	table::iterator it;
	for(it = pool.begin(); it != pool.end(); it++) {
		it->second = false;
	}
}

void laser::update_all(float delta_time) {
	table::iterator it;
	for(it = pool.begin(); it != pool.end(); it++) {
		if(it->second) {
			it->first->update(delta_time);
		}
	}
}

void laser::draw_all() {
	table::iterator it;
	for(it = pool.begin(); it != pool.end(); it++) {
		if(it->second) {
			it->first->draw();
		}
	}
}
