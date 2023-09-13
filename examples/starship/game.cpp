#include "game.hpp"

const float BG_SCROLL_SPEED = 0.25F;

game::game() : rge::engine() {
	camera = rge::camera::create();
}

void game::get_default_window_params(std::string& title, int& width, int& height, bool& fullscreen) {
	title = "Starship";
	width = 800;
	height = 600;
	fullscreen = false;
}

void game::on_init() {
	camera->set_orthographic(-8, 8, 6, -6, 0.0F, 100.0F);
	camera->transform->position = rge::vec3(0, 0, -1);

	get_renderer()->set_camera(camera);

	ship = new spaceship();

	title_sprite = rge::sprite::create();
	title_sprite->texture = rge::texture::load("res/title.png");
	title_sprite->pixels_per_unit = 16;

	bg_sprite_0 = rge::sprite::create();
	bg_sprite_0->texture = rge::texture::load("res/background.png");
	bg_sprite_0->pixels_per_unit = 16;
	bg_sprite_1 = rge::sprite::create();
	bg_sprite_1->texture = rge::texture::load("res/background.png");
	bg_sprite_1->pixels_per_unit = 16;
}

void game::on_start() {
	// state = game_state::MAIN_MENU;
	start_game();
}

void game::on_update(float delta_time) {
	if(state == game_state::MAIN_MENU) {
		if(rge::input::was_pressed(rge::input::KEY_ESC)) {
			exit();
		} else if(rge::input::was_pressed(rge::input::ANY)) {
			start_game();
		}
	} else if(state == game_state::IN_GAME) {
		asteroid_countdown -= delta_time;
		if(asteroid_countdown <= 0.0F) {
			set_rand_asteroid_wait();

			asteroid::create();
		}

		scroll_bg(delta_time);
		ship->update(delta_time);
		asteroid::update_all(delta_time);
		laser::update_all(delta_time);
	} else if(state == game_state::END_SCREEN) {
		if(rge::input::was_pressed(rge::input::KEY_ESC)) {
			state = game_state::MAIN_MENU;
		}
	}
}

void game::on_render() {
	get_renderer()->clear(rge::color(0.2F, 0.2F, 0.2F));

	if(state == game_state::MAIN_MENU) {
		get_renderer()->draw(*title_sprite);
		get_renderer()->draw(*press_key_sprite_0);
	} else if(state == game_state::IN_GAME) {
		get_renderer()->draw(*bg_sprite_0);
		get_renderer()->draw(*bg_sprite_1);
		
		asteroid::draw_all();
		laser::draw_all();
		ship->draw();
	} else if(state == game_state::END_SCREEN) {

	}
}

void game::start_game() {
	state = game_state::IN_GAME;
	random = rge::random();
	bg_scroll_0 = 0.0F;
	bg_scroll_1 = 0.5F;
	set_rand_asteroid_wait();
	ship->reset();
}

void game::scroll_bg(float delta_time) {
	bg_scroll_0 += delta_time * BG_SCROLL_SPEED;
	bg_scroll_1 += delta_time * BG_SCROLL_SPEED;
	if(bg_scroll_0 > 1) bg_scroll_0 -= 1;
	if(bg_scroll_1 > 1) bg_scroll_1 -= 1;

	bg_sprite_0->transform->position = rge::vec3(-8, rge::math::lerp(6, -18, bg_scroll_0), -BACKGROUND_LAYER);
	bg_sprite_1->transform->position = rge::vec3(-8, rge::math::lerp(6, -18, bg_scroll_1), -BACKGROUND_LAYER);
}

void game::set_rand_asteroid_wait() {
	asteroid_countdown = random.range(1.0F, 2.0F);
}
