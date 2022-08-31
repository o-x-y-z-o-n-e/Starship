#include "video/sprite.h"
#include "video/texture.h"
#include "video/renderer.h"
#include "util/debug.h"


//------------------------------------------------------------------------------


void set_texture(sprite_t* sprite, texture_t* texture) {
	if(texture == NULL) {
		sprite->texture = NULL;
		sprite->section = (rect_t){ 0, 0, 0, 0 };
		return;
	}

	sprite->texture = texture;
	sprite->section = (rect_t){ 0, 0, texture->width, texture->height };
}


//------------------------------------------------------------------------------


sprite_t* create_sprite(entity_t* entity, uint8_t layer) {
	if(entity == NULL) {
		log_error("No entity passed");
		return NULL;
	}

	registry_t* registry = get_layer(layer);
	if(registry == NULL)
		return NULL;

	return (sprite_t*)create_component(entity, registry);
}


//------------------------------------------------------------------------------


texture_t* get_texture(const sprite_t* sprite) {
	if(sprite == NULL)
		return NULL;

	return sprite->texture;
}


//------------------------------------------------------------------------------


void set_sprite_section(sprite_t* sprite, rect_t section) {
	if(sprite == NULL)
		return;

	if(sprite->texture == NULL) {
		sprite->section = (rect_t){ 0,0,0,0 };
		return;
	}

	if(section.x < 0)
		section.x = 0;

	if(section.y < 0)
		section.y = 0;

	if(section.x + section.width > sprite->texture->width)
		section.width = sprite->texture->width - section.x;

	if(section.y + section.height > sprite->texture->height)
		section.height = sprite->texture->height - section.y;

	sprite->section = section;
}