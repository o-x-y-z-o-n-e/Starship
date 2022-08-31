#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>
#include "util/types.h"

typedef struct registry_t registry_t;

typedef struct pixel_t {
	union {
		struct {
			uint8_t r, g, b, a;
		};
		uint32_t p;
	};
} pixel_t;

typedef struct viewport_t {
	pixel_t* buffer;
	uint16_t width;
	uint16_t height;
	uint16_t scale;
} viewport_t;

typedef struct sprite_t sprite_t;

#define COLOR_RGB(R, G, B) (pixel_t){R, G, B, 255}
#define COLOR_RGBA(R, G, B, A) (pixel_t){R, G, B, A}

// Init.
int init_renderer();

// Viewpoint.
uint16_t get_view_width();
uint16_t get_view_height();
void set_viewport_size(uint16_t width, uint16_t height);
void set_camera_location(point_t location);

// Drawing.
void draw_sprite(const sprite_t* sprite, float delta);
void draw_all();
void set_clear_color(pixel_t color);

// Misc.
registry_t* get_layer(uint8_t layer);

#endif