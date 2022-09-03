#ifndef SPRITE_H
#define SPRITE_H

#include "util/types.h"
#include "world/scene.h"
#include <stdbool.h>

#define TYPE_SPRITE 1

typedef struct texture_t texture_t;

typedef struct sprite_t {
	entity_t* owner;
    texture_t* texture;
    rect_t section;
    point_t offset;
} sprite_t;

sprite_t* create_sprite(entity_t* entity, uint8_t layer);
void set_texture(sprite_t* sprite, texture_t* texture);
texture_t* get_texture(const sprite_t* sprite);
void set_sprite_section(sprite_t* sprite, rect_t section);
void set_sprite_centered(sprite_t* sprite, bool centered);

#endif