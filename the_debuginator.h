
/*
the_debuginator.h - v0.01 - public domain - Anders Elfgren @srekel, 2017

# THE DEBUGINATOR

A super sweet hierarchical scrollable accordion debug menu intended for games.

See github for latest version: https://github.com/Srekel/the-debuginator

## Usage

In *ONE* source file, put:

```C
#define DEBUGINATOR_IMPLEMENTATION

// Define any of these if you wish to override them.
// #define DEBUGINATOR_assert
// #define DEBUGINATOR_memcpy
// #define DEBUGINATOR_fabs

#include "the_debuginator.h"
```

Other source files should just include the_debuginator.h

## Notes

See the accompanying SDL demo and unit test projects for references on how to use it.

ASCII banners generated using
http://patorjk.com/software/taag/#p=display&f=ANSI%20Shadow&t=update

## License

See end of file for license information.

*/

#ifndef INCLUDE_THE_DEBUGINATOR_H
#define INCLUDE_THE_DEBUGINATOR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DebuginatorItem DebuginatorItem;

#ifndef DEBUGINATOR_max_title_length
#define DEBUGINATOR_max_title_length 20
#endif

#ifndef DEBUGINATOR_max_themes
#define DEBUGINATOR_max_themes 16
#endif


// TODO C99-ify these
typedef struct DebuginatorVector2 {
	float x;
	float y;
} DebuginatorVector2;

typedef struct DebuginatorColor {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} DebuginatorColor;

typedef struct DebuginatorFont {
	void* userdata;
	int size;
	bool bold;
	bool italic;
} DebuginatorFont;

typedef enum DebuginatorDrawTypes {
	DEBUGINATOR_Background,
	DEBUGINATOR_FolderTitle,
	DEBUGINATOR_ItemTitle,
	DEBUGINATOR_ItemTitleOverridden,
	DEBUGINATOR_ItemTitleHot,
	DEBUGINATOR_ItemTitleActive,
	DEBUGINATOR_ItemTitleActive1,
	DEBUGINATOR_ItemTitleActive2,
	DEBUGINATOR_ItemDescription,
	DEBUGINATOR_LineHighlight,
	DEBUGINATOR_ItemValueDefault,
	DEBUGINATOR_ItemValueOverridden,
	DEBUGINATOR_ItemValueHot,
	DEBUGINATOR_NumDrawTypes
} DebuginatorDrawTypes;

typedef struct DebuginatorTheme {
	DebuginatorColor colors[DEBUGINATOR_NumDrawTypes];
	DebuginatorFont fonts[DEBUGINATOR_NumDrawTypes];
} DebuginatorTheme;

typedef struct DebuginatorFolderData {
	struct DebuginatorItem* first_child;
	struct DebuginatorItem* hot_child;
} DebuginatorFolderData;

typedef struct DebuginatorItem DebuginatorItem;
typedef struct TheDebuginator TheDebuginator;

typedef void(*DebuginatorDrawTextCallback)
	(const char* text, DebuginatorVector2* position, DebuginatorColor* color, DebuginatorFont* font, void* userdata);
typedef void(*DebuginatorDrawRectCallback)
	(DebuginatorVector2 position, DebuginatorVector2 size, DebuginatorColor color, void* userdata);
typedef const char* (*DebuginatorWordWrapCallback)
	(const char* text, DebuginatorFont font, float max_width, char* buffer, int buffer_size, void* userdata);

typedef void(*DebuginatorItemQuickDrawCallback)(DebuginatorItem* item, void* data);
typedef void(*DebuginatorOnItemChangedCallback)(DebuginatorItem* item, void* value, const char* value_title);

typedef enum DebuginatorItemEditorDataType {
	DEBUGINATOR_EditTypeArray, // The default
	DEBUGINATOR_EditTypeBoolean,
	/*DEBUGINATOR_EditTypeUserType1,
	...
	DEBUGINATOR_EditTypeUserTypeN,*/
	DEBUGINATOR_EditTypeCount = 16,
} DebuginatorItemEditorDataType;

typedef struct DebuginatorLeafData {
	const char* description;

	bool is_active;
	char* hot_value;
	int hot_index;
	int active_index;

	const char** value_titles;
	const char** value_descriptions;
	void* values;

	int num_values;
	size_t array_element_size;

	DebuginatorOnItemChangedCallback on_item_changed_callback;
	DebuginatorItemEditorDataType edit_type;
	float draw_t;
} DebuginatorLeafData;

typedef struct DebuginatorItem {
	char title[20];
	bool is_folder;
	void* user_data;
	float total_height; // Including self and children

	DebuginatorItem* prev_sibling;
	DebuginatorItem* next_sibling;
	DebuginatorItem* parent;

	union {
		DebuginatorLeafData leaf;
		DebuginatorFolderData folder;
		#pragma warning(suppress: 4201) // Unnamed union
	};
} DebuginatorItem;

typedef enum DebuginatorAnimationType {
	DEBUGINATOR_ItemActivate
} DebuginatorAnimationType;

typedef struct DebuginatorItemEditorData {
	void(*quick_draw)(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position);
	float(*expanded_height)(DebuginatorItem* item, void* userdata);
	void(*expanded_draw)(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position);
} DebuginatorItemEditorData;

typedef struct DebuginatorAnimation {
	DebuginatorAnimationType type;
	union {
		struct {
			DebuginatorItem* item;
			int value_index;
			DebuginatorVector2 start_pos;
		} item_activate;
	} data;

	float duration;
	float time;
} DebuginatorAnimation;

typedef struct TheDebuginatorConfig {
	bool create_default_debuginator_items;

	size_t item_buffer_capacity;
	DebuginatorItem* item_buffer;

	DebuginatorTheme themes[16];

	DebuginatorItemEditorData edit_types[DEBUGINATOR_EditTypeCount];

	void* draw_user_data;
	DebuginatorDrawTextCallback draw_text;
	DebuginatorDrawRectCallback draw_rect;
	DebuginatorWordWrapCallback word_wrap;

	DebuginatorVector2 size; // Might not be needed in the future
	DebuginatorVector2 screen_resolution;
	bool left_aligned;
	int open_direction;
	float focus_height;
} TheDebuginatorConfig;

typedef struct TheDebuginator {
	DebuginatorItem* root;
	DebuginatorItem* hot_item;

	size_t item_buffer_capacity;
	size_t item_buffer_size;
	DebuginatorItem* item_buffer;

	size_t free_list_size;
	size_t free_list[256];

	bool is_open;
	float openness_timer; // range [0,1]
	float openness; // range [0,1]

	DebuginatorTheme themes[16];
	DebuginatorTheme theme; // current theme
	int theme_index;

	DebuginatorItemEditorData edit_types[DEBUGINATOR_EditTypeCount];

	float dt;
	float draw_timer;
	void* draw_user_data;
	DebuginatorDrawTextCallback draw_text;
	DebuginatorDrawRectCallback draw_rect;
	DebuginatorWordWrapCallback word_wrap;

	DebuginatorVector2 size;
	DebuginatorVector2 root_position;
	DebuginatorVector2 screen_resolution;
	bool left_aligned;
	int open_direction;
	float focus_height;
	float current_height_offset;

	DebuginatorAnimation animations[8];
	int animation_count;
} TheDebuginator;


extern DebuginatorItem* debuginator_create_array_item(TheDebuginator* debuginator,
	DebuginatorItem* parent, const char* path, const char* description,
	DebuginatorOnItemChangedCallback on_item_changed_callback, void* user_data,
	const char** value_titles, void* values, int num_values, size_t value_size);

extern void debuginator_create_bool_item(TheDebuginator* debuginator, const char* path, const char* description, void* user_data);

extern DebuginatorItem* debuginator_new_folder_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* title, size_t title_length);
extern DebuginatorItem* debuginator_get_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* path, bool create_if_not_exist);
extern void debuginator_set_hot_item(TheDebuginator* debuginator, const char* path);
extern void debuginator_remove_item(TheDebuginator* debuginator, const char* path);
#ifdef __cplusplus
}
#endif

#ifdef DEBUGINATOR_IMPLEMENTATION

#ifndef DEBUGINATOR_assert
#include <assert.h>
#define DEBUGINATOR_assert assert;
#endif

#ifndef DEBUGINATOR_memcpy
#include <string.h>
#define DEBUGINATOR_memcpy memcpy
#endif

#ifndef DEBUGINATOR_fabs
#include <math.h>
#define DEBUGINATOR_fabs fabs
#endif

#ifndef DEBUGINATOR_sin
#include <math.h>
#define DEBUGINATOR_sin sin
#endif

#ifndef __cplusplus
#include <stdbool.h>
#endif
//
//#ifndef DEBUGINATOR_FREE_LIST_CAPACITY
//#define DEBUGINATOR_FREE_LIST_CAPACITY 256
//#endif
//
//#ifndef DEBUGINATOR_max_title_length
//#define DEBUGINATOR_max_title_length 20
//#endif
//
//#ifndef DEBUGINATOR_max_themes
//#define DEBUGINATOR_max_themes 16
//#endif

#ifndef DEBUGINATOR_debug_print
#define DEBUGINATOR_debug_print
#endif

DebuginatorVector2 debuginator__vector2(float x, float y) {
	DebuginatorVector2 v; v.x = x; v.y = y;
	return v;
}

DebuginatorColor debuginator__color(int r, int g, int b, int a) {
	DEBUGINATOR_assert(0 <= r && r <= 255);
	DEBUGINATOR_assert(0 <= g && g <= 255);
	DEBUGINATOR_assert(0 <= b && b <= 255);
	DEBUGINATOR_assert(0 <= a && a <= 255);
	DebuginatorColor c;
	c.r = (unsigned char)r; c.g = (unsigned char)g;
	c.b = (unsigned char)b; c.a = (unsigned char)a;
	return c;
}

float debuginator__ease_out(float t, float start_value, float change, float duration) {
	t /= duration;
	return -change * t * (t - 2) + start_value;
}

float debuginator__lerp(float a, float b, float t) {
	return a * (1 - t) + b * t;
}

void debuginator__quick_draw_default(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	if (item->leaf.num_values > 0) {
		DebuginatorVector2 value_offset = *position;
		if (debuginator->left_aligned) {
			value_offset.x = debuginator->openness * debuginator->size.x - 200;
		}
		else {
			value_offset.x = debuginator->screen_resolution.x + debuginator->size.x * (1 - debuginator->openness) - 200;
		}

		bool is_overriden = item->leaf.active_index != 0;
		unsigned default_color_index = is_overriden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemTitle;
		debuginator->draw_text(item->leaf.value_titles[item->leaf.active_index], &value_offset, &debuginator->theme.colors[default_color_index], &debuginator->theme.fonts[DEBUGINATOR_ItemTitle], debuginator->draw_user_data);
	}
}


void debuginator__expanded_draw_default(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	for (size_t i = 0; i < item->leaf.num_values; i++) {
		position->y += 30;

		if (debuginator->hot_item == item && item->leaf.hot_index == i) {
			DebuginatorVector2 pos = debuginator__vector2(0, position->y - 5);
			DebuginatorVector2 size = debuginator__vector2(500, 30);
			if (!debuginator->left_aligned) {
				pos.x = debuginator->screen_resolution.x - debuginator->openness * debuginator->size.x;
			}
			debuginator->draw_rect(pos, size, debuginator->theme.colors[DEBUGINATOR_LineHighlight], debuginator->draw_user_data);
		}

		const char* value_title = item->leaf.value_titles[i];
		bool value_hot = i == item->leaf.hot_index;
		bool value_overridden = i == item->leaf.active_index;
		unsigned value_color_index = value_hot ? DEBUGINATOR_ItemValueHot : (value_overridden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemValueDefault);
		debuginator->draw_text(value_title, position, &debuginator->theme.colors[value_color_index], &debuginator->theme.fonts[value_hot ? DEBUGINATOR_ItemTitle : DEBUGINATOR_ItemTitle], debuginator->draw_user_data);
	}
}

void debuginator__quick_draw_boolean(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	DebuginatorVector2 pos = debuginator__vector2(debuginator->size.x * debuginator->openness - 200, position->y);
	DebuginatorVector2 size = debuginator__vector2(50, 20);
	DebuginatorColor background = debuginator__color(0, 0, 0, 100);
	debuginator->draw_rect(pos, size, background, debuginator->draw_user_data);

	item->leaf.draw_t += debuginator->dt;
	if (item->leaf.draw_t > 1) {
		item->leaf.draw_t = 1;
	}

	DebuginatorVector2 slider_pos = pos;
	if (item->leaf.active_index == 0) {
		//slider_pos.x = debuginator__lerp(pos.x + 27, pos.x + 2, item->leaf.draw_t);
		slider_pos.x = debuginator__ease_out(item->leaf.draw_t, pos.x + 27, -25, 1);
	}
	else {
		//slider_pos.x = debuginator__lerp(pos.x + 2, pos.x + 27, item->leaf.draw_t);
		slider_pos.x = debuginator__ease_out(item->leaf.draw_t, pos.x + 2, 25, 1);
	}
	//slider_pos.x += item->leaf.active_index == 0 ? 2 : 27;
	slider_pos.y += 2;
	size.x = 21;
	size.y = 16;
	DebuginatorColor slider = item->leaf.active_index == 0 ? debuginator__color(200, 100, 100, 200) : debuginator__color(100, 200, 100, 200);

	debuginator->draw_rect(slider_pos, size, slider, debuginator->draw_user_data);
}

void debuginator__expanded_draw_boolean(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2* position) {
	for (size_t i = 0; i < item->leaf.num_values; i++) {
		position->y += 30;

		if (debuginator->hot_item == item && item->leaf.hot_index == i) {
			DebuginatorVector2 pos = debuginator__vector2(0, position->y - 5);
			DebuginatorVector2 size = debuginator__vector2(500, 30);
			if (!debuginator->left_aligned) {
				pos.x = debuginator->screen_resolution.x - debuginator->openness * debuginator->size.x;
			}
			debuginator->draw_rect(pos, size, debuginator->theme.colors[DEBUGINATOR_LineHighlight], debuginator->draw_user_data);
		}

		const char* value_title = item->leaf.value_titles[i];
		bool value_hot = i == item->leaf.hot_index;
		bool value_overridden = i == item->leaf.active_index;
		unsigned value_color_index = value_hot ? DEBUGINATOR_ItemValueHot : (value_overridden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemValueDefault);
		debuginator->draw_text(value_title, position, &debuginator->theme.colors[value_color_index], &debuginator->theme.fonts[value_hot ? DEBUGINATOR_ItemTitle : DEBUGINATOR_ItemTitle], debuginator->draw_user_data);
	}
}

DebuginatorAnimation* debuginator__get_free_animation(TheDebuginator* debuginator) {
	if (debuginator->animation_count == sizeof(debuginator->animations) / sizeof(*debuginator->animations)) {
		return NULL;
	}

	DebuginatorAnimation* animation = &debuginator->animations[debuginator->animation_count++];
	memset(animation, 0, sizeof(*animation));
	return animation;
}

DebuginatorItem* debuginator_get_free_item(TheDebuginator* debuginator) {
	DebuginatorItem* item;
	if (debuginator->free_list_size > 0) {
		size_t free_index = debuginator->free_list[--debuginator->free_list_size];
		DEBUGINATOR_assert(0 <= free_index && free_index < debuginator->item_buffer_capacity);
		item = &debuginator->item_buffer[free_index];
	}
	else {
		DEBUGINATOR_assert(debuginator->item_buffer_size < debuginator->item_buffer_capacity);
		item = &debuginator->item_buffer[debuginator->item_buffer_size++];
	}

	memset(item, 0, sizeof(*item));
	return item;
}

void debuginator__set_total_height(DebuginatorItem* item, float height) {
	if (item->total_height == height) {
		return;
	}

	float diff = height - item->total_height;
	item->total_height = height;
	if (item->parent) {
		debuginator__set_total_height(item->parent, item->parent->total_height + diff);
	}
}

void debuginator__on_change_theme(DebuginatorItem* item, void* value, const char* value_title) {
	(void)value_title;
	TheDebuginator* debuginator = (TheDebuginator*)item->user_data;
	debuginator->theme_index = *(int*)value;
	debuginator->theme = debuginator->themes[debuginator->theme_index];
}

void debuginator_set_title(DebuginatorItem* item, const char* title, size_t title_length) {
	if (title_length == 0) {
		title_length = strlen(title);
	}

	if (title_length >= DEBUGINATOR_max_title_length) {
#pragma warning(suppress: 4996)
		strncpy(item->title, title, DEBUGINATOR_max_title_length - 3);
		item->title[DEBUGINATOR_max_title_length - 3] = '.';
		item->title[DEBUGINATOR_max_title_length - 2] = '.';
		item->title[DEBUGINATOR_max_title_length - 1] = '\0';
	}
	else {
#pragma warning(suppress: 4996)
		strncpy(item->title, title, title_length);
	}
}

void debuginator_set_parent(DebuginatorItem* item, DebuginatorItem* parent) {
	if (parent == NULL)
		return;

	DEBUGINATOR_assert(item->parent == NULL || item->parent == parent);
	item->parent = parent;
	if (parent->folder.first_child == NULL) {
		parent->folder.first_child = item;
	}
	else {
		DebuginatorItem* last_sibling = parent->folder.first_child;
		while (last_sibling != NULL)
		{
			if (last_sibling == item) {
				// Item was already in parent
				return;
			}

			// TODO do alphanumerical comparison here for sorting

			if (last_sibling->next_sibling == NULL) {
				// Found the last child, set item as the new last one
				last_sibling->next_sibling = item;
				item->prev_sibling = last_sibling;
				return;
			}

			last_sibling = last_sibling->next_sibling;
		}
	}
}

DebuginatorItem* debuginator_new_folder_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* title, size_t title_length) {
	DEBUGINATOR_assert(debuginator->item_buffer_size < debuginator->item_buffer_capacity);
	DebuginatorItem* folder_item;
	if (debuginator->free_list_size > 0) {
		folder_item = &debuginator->item_buffer[--debuginator->free_list_size];
	}
	else {
		folder_item = &debuginator->item_buffer[debuginator->item_buffer_size++];
	}

	folder_item->is_folder = true;
	debuginator_set_title(folder_item, title, title_length);
	debuginator_set_parent(folder_item, parent);
	debuginator__set_total_height(folder_item, 30);
	return folder_item;
}

DebuginatorItem* debuginator_get_item(TheDebuginator* debuginator, DebuginatorItem* parent, const char* path, bool create_if_not_exist) {
	parent = parent == NULL ? debuginator->root : parent;
	const char* temp_path = path;
	while (true) {
		const char* next_slash = strchr(temp_path, '/');
		size_t path_part_length = next_slash ? next_slash - temp_path : strlen(temp_path);

		DebuginatorItem* current_item = NULL;
		DebuginatorItem* parent_child = parent->folder.first_child;
		while (parent_child) {
			const char* item_title = parent_child->title;
			size_t title_length = strlen(item_title); // strlen :(
			if (path_part_length >= DEBUGINATOR_max_title_length
				&& title_length == DEBUGINATOR_max_title_length - 1
				&& item_title[DEBUGINATOR_max_title_length - 2] == '.'
				&& item_title[DEBUGINATOR_max_title_length - 3] == '.') {
				path_part_length = title_length = DEBUGINATOR_max_title_length - 3;
			}

			if (title_length == path_part_length && memcmp(parent_child->title, temp_path, title_length * sizeof(char)) == 0) {
				current_item = parent_child;
				break;
			}

			parent_child = parent_child->next_sibling;
		}

		if (current_item == NULL && !create_if_not_exist) {
			return NULL;
		}

		// If current_item is set, it means the item already existed and we're just going to reuse it
		if (next_slash == NULL) {
			// Found the last part of the path
			if (current_item == NULL) {
				current_item = debuginator_get_free_item(debuginator);
				debuginator_set_title(current_item, temp_path, 0);
				debuginator_set_parent(current_item, parent);
			}

			return current_item;
		}
		else {
			// Found a folder
			if (current_item == NULL) {
				// Parent item doesn't exist yet
				parent = debuginator_new_folder_item(debuginator, parent, temp_path, next_slash - temp_path);
			}
			else {
				parent = current_item;
			}
			temp_path = next_slash + 1;
		}
	}

	DEBUGINATOR_assert(false);
	return NULL;
}

DebuginatorItem* debuginator_create_array_item(TheDebuginator* debuginator,
	DebuginatorItem* parent, const char* path, const char* description,
	DebuginatorOnItemChangedCallback on_item_changed_callback, void* user_data,
	const char** value_titles, void* values, int num_values, size_t value_size) {

	DebuginatorItem* item = debuginator_get_item(debuginator, parent, path, true);
	item->is_folder = false;
	item->leaf.num_values = num_values;
	item->leaf.values = values;
	item->leaf.array_element_size = value_size;
	item->leaf.value_titles = value_titles;
	item->leaf.on_item_changed_callback = on_item_changed_callback;
	item->user_data = user_data;
	item->leaf.description = description;
	debuginator__set_total_height(item, 30);

	if (item->leaf.hot_index >= num_values) {
		item->leaf.hot_index = num_values - 1;
	}

	if (debuginator->hot_item == NULL && !item->is_folder) {
		item->parent->folder.hot_child = item;
		debuginator->hot_item = item;
	}

	//TODO preserve hot item
	return item;
}

void debuginator_set_hot_item(TheDebuginator* debuginator, const char* path) {
	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, path, false);
	if (item == NULL) {
		return;
	}

	debuginator->hot_item = item;
	item->parent->folder.hot_child = item;
}

void debuginator_remove_item(TheDebuginator* debuginator, const char* path) {
	DebuginatorItem* item = debuginator_get_item(debuginator, NULL, path, false);
	if (item == NULL) {
		return;
	}

	if (item->prev_sibling) {
		item->prev_sibling->next_sibling = item->next_sibling;
	}
	if (item->next_sibling) {
		item->next_sibling->prev_sibling = item->prev_sibling;
	}

	DebuginatorItem* parent = item->parent;
	if (parent->folder.hot_child == item) {
		if (item->next_sibling != NULL) {
			parent->folder.hot_child = item->next_sibling;
		}
		else if (item->prev_sibling != NULL) {
			parent->folder.hot_child = item->prev_sibling;
		}
		else {
			parent->folder.hot_child = NULL;
		}

		if (parent->folder.first_child == item) {
			parent->folder.first_child = item->next_sibling;
		}
	}

	if (debuginator->hot_item == item) {
		if (parent->folder.hot_child == NULL) {
			debuginator->hot_item = parent;
		}
		else {
			debuginator->hot_item = parent->folder.hot_child;
		}
	}

	debuginator__set_total_height(item->parent, item->parent->total_height - 30);
}

//██╗███╗   ██╗██╗████████╗
//██║████╗  ██║██║╚══██╔══╝
//██║██╔██╗ ██║██║   ██║
//██║██║╚██╗██║██║   ██║
//██║██║ ╚████║██║   ██║
//╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝

void debuginator_get_default_config(TheDebuginatorConfig* config) {
	memset(config, 0, sizeof(*config));

	config->create_default_debuginator_items = true;
	config->open_direction = 1;
	config->focus_height = 0.65f;
	config->left_aligned = true;

	// Initialize default themes
	DebuginatorTheme* themes = config->themes;

	// Classic theme
	themes[0].colors[DEBUGINATOR_Background] = debuginator__color(25, 50, 25, 220);
	themes[0].colors[DEBUGINATOR_FolderTitle] = debuginator__color(255, 255, 255, 255);
	themes[0].colors[DEBUGINATOR_ItemTitle] = debuginator__color(120, 120, 0, 250);
	themes[0].colors[DEBUGINATOR_ItemTitleOverridden] = debuginator__color(200, 200, 0, 255);
	themes[0].colors[DEBUGINATOR_ItemTitleHot] = debuginator__color(230, 230, 200, 255);
	themes[0].colors[DEBUGINATOR_ItemTitleActive] = debuginator__color(100, 255, 100, 255);
	themes[0].colors[DEBUGINATOR_ItemTitleActive1] = debuginator__color(100, 255, 100, 255);
	themes[0].colors[DEBUGINATOR_ItemTitleActive2] = debuginator__color(200, 255, 200, 255);
	themes[0].colors[DEBUGINATOR_ItemDescription] = debuginator__color(150, 150, 150, 255);
	themes[0].colors[DEBUGINATOR_ItemValueDefault] = debuginator__color(50, 150, 50, 200);
	themes[0].colors[DEBUGINATOR_ItemValueOverridden] = debuginator__color(100, 255, 100, 200);
	themes[0].colors[DEBUGINATOR_ItemValueHot] = debuginator__color(100, 255, 100, 200);
	themes[0].colors[DEBUGINATOR_LineHighlight] = debuginator__color(100, 100, 50, 150);
	themes[0].fonts[DEBUGINATOR_ItemDescription].italic = true;

	// Neon theme
	themes[1].colors[DEBUGINATOR_Background] = debuginator__color(15, 15, 30, 220);
	themes[1].colors[DEBUGINATOR_FolderTitle] = debuginator__color(255, 255, 255, 255);
	themes[1].colors[DEBUGINATOR_ItemTitle] = debuginator__color(120, 120, 180, 250);
	themes[1].colors[DEBUGINATOR_ItemTitleOverridden] = debuginator__color(150, 150, 200, 255);
	themes[1].colors[DEBUGINATOR_ItemTitleHot] = debuginator__color(220, 220, 250, 255);
	themes[1].colors[DEBUGINATOR_ItemTitleActive] = debuginator__color(100, 100, 255, 255);
	themes[1].colors[DEBUGINATOR_ItemTitleActive1] = debuginator__color(100, 255, 100, 255);
	themes[1].colors[DEBUGINATOR_ItemTitleActive2] = debuginator__color(200, 200, 255, 255);
	themes[1].colors[DEBUGINATOR_ItemDescription] = debuginator__color(150, 150, 150, 255);
	themes[1].colors[DEBUGINATOR_ItemValueDefault] = debuginator__color(50, 50, 150, 200);
	themes[1].colors[DEBUGINATOR_ItemValueOverridden] = debuginator__color(100, 100, 255, 200);
	themes[1].colors[DEBUGINATOR_ItemValueHot] = debuginator__color(100, 100, 255, 200);
	themes[1].colors[DEBUGINATOR_LineHighlight] = debuginator__color(70, 70, 130, 150);

	// Black & White theme
	themes[2].colors[DEBUGINATOR_Background] = debuginator__color(25, 25, 25, 220);
	themes[2].colors[DEBUGINATOR_FolderTitle] = debuginator__color(255, 255, 255, 255);
	themes[2].colors[DEBUGINATOR_ItemTitle] = debuginator__color(120, 120, 120, 250);
	themes[2].colors[DEBUGINATOR_ItemTitleOverridden] = debuginator__color(200, 200, 200, 255);
	themes[2].colors[DEBUGINATOR_ItemTitleHot] = debuginator__color(230, 230, 230, 255);
	themes[2].colors[DEBUGINATOR_ItemTitleActive] = debuginator__color(100, 100, 100, 255);
	themes[2].colors[DEBUGINATOR_ItemTitleActive1] = debuginator__color(100, 100, 100, 255);
	themes[2].colors[DEBUGINATOR_ItemTitleActive2] = debuginator__color(200, 200, 200, 255);
	themes[2].colors[DEBUGINATOR_ItemDescription] = debuginator__color(150, 150, 150, 255);
	themes[2].colors[DEBUGINATOR_ItemValueDefault] = debuginator__color(100, 100, 100, 200);
	themes[2].colors[DEBUGINATOR_ItemValueOverridden] = debuginator__color(200, 200, 200, 200);
	themes[2].colors[DEBUGINATOR_ItemValueHot] = debuginator__color(255, 255, 255, 200);
	themes[2].colors[DEBUGINATOR_LineHighlight] = debuginator__color(100, 100, 100, 150);

	// Beige
	themes[3].colors[DEBUGINATOR_Background] = debuginator__color(255, 240, 220, 220);
	themes[3].colors[DEBUGINATOR_FolderTitle] = debuginator__color(0, 0, 0, 255);
	themes[3].colors[DEBUGINATOR_ItemTitle] = debuginator__color(120, 120, 120, 250);
	themes[3].colors[DEBUGINATOR_ItemTitleOverridden] = debuginator__color(200, 200, 200, 255);
	themes[3].colors[DEBUGINATOR_ItemTitleHot] = debuginator__color(230, 230, 230, 255);
	themes[3].colors[DEBUGINATOR_ItemTitleActive] = debuginator__color(100, 100, 100, 255);
	themes[3].colors[DEBUGINATOR_ItemTitleActive1] = debuginator__color(100, 100, 100, 255);
	themes[3].colors[DEBUGINATOR_ItemTitleActive2] = debuginator__color(0, 0, 0, 255);
	themes[3].colors[DEBUGINATOR_ItemDescription] = debuginator__color(150, 150, 150, 255);
	themes[3].colors[DEBUGINATOR_ItemValueDefault] = debuginator__color(100, 100, 100, 200);
	themes[3].colors[DEBUGINATOR_ItemValueOverridden] = debuginator__color(200, 200, 200, 200);
	themes[3].colors[DEBUGINATOR_ItemValueHot] = debuginator__color(255, 255, 255, 200);
	themes[3].colors[DEBUGINATOR_LineHighlight] = debuginator__color(100, 100, 100, 150);

	config->edit_types[DEBUGINATOR_EditTypeArray].quick_draw = debuginator__quick_draw_default;
	config->edit_types[DEBUGINATOR_EditTypeArray].expanded_draw = debuginator__expanded_draw_default;
	config->edit_types[DEBUGINATOR_EditTypeBoolean].quick_draw = debuginator__quick_draw_boolean;
	config->edit_types[DEBUGINATOR_EditTypeBoolean].expanded_draw = debuginator__expanded_draw_boolean;
}

void debuginator_create(TheDebuginatorConfig* config, TheDebuginator* debuginator) {
	DEBUGINATOR_assert(config->draw_rect != NULL);
	DEBUGINATOR_assert(config->draw_text != NULL);
	//DEBUGINATOR_assert(config->draw_user_data);
	DEBUGINATOR_assert(config->word_wrap != NULL);
	DEBUGINATOR_assert(config->item_buffer != NULL);
	DEBUGINATOR_assert(config->item_buffer_capacity > 0);
	//DEBUGINATOR_assert(config->open_direction == -1 || config->open_direction == 1);
	DEBUGINATOR_assert(config->size.x > 0 && config->size.y > 0);
	DEBUGINATOR_assert(config->screen_resolution.x > 0 && config->screen_resolution.y > 0);

	memset(debuginator, 0, sizeof(*debuginator));

	debuginator->item_buffer_capacity = config->item_buffer_capacity;
	debuginator->item_buffer = config->item_buffer;
	memset(debuginator->item_buffer, 0, sizeof(DebuginatorItem) * debuginator->item_buffer_capacity);

	debuginator->draw_rect = config->draw_rect;
	debuginator->draw_text = config->draw_text;
	debuginator->word_wrap = config->word_wrap;
	debuginator->draw_user_data = config->draw_user_data;

	debuginator->size = config->size;
	debuginator->open_direction = config->open_direction;
	debuginator->focus_height = config->focus_height;
	debuginator->screen_resolution = config->screen_resolution;
	debuginator->left_aligned= config->left_aligned;

	if (debuginator->left_aligned) {
		debuginator->open_direction = 1;
		debuginator->root_position.x = -debuginator->size.x;
	}
	else {
		debuginator->open_direction = -1;
		debuginator->root_position.x = debuginator->screen_resolution.x;
	}

	memcpy(debuginator->edit_types, config->edit_types, sizeof(debuginator->edit_types));
	memcpy(debuginator->themes, config->themes, sizeof(debuginator->themes));
	debuginator->theme_index = 0;
	debuginator->theme = debuginator->themes[0];

	// Create root
	DebuginatorItem* item = debuginator_new_folder_item(debuginator, NULL, "Menu Root", 0);
	debuginator->root = item;

	if (config->create_default_debuginator_items) {
		{
			debuginator_create_array_item(debuginator, NULL, "Debuginator/Help",
				"The Debuginator is a debug menu. With a keyboard, you open it with Right Arrow and close it with Left Arrow. You use those keys, plus Up/Down arrows to navigate. Right Arrow is also used to change value on a menu item.", NULL, NULL,
				NULL, NULL, 0, 0);
		}
		{
			static int theme_indices[4] = { 0, 1, 2, 3 };
			static const char* string_titles[4] = { "Classic", "Blue", "Black & White", "Beige" };
			debuginator_create_array_item(debuginator, NULL, "Debuginator/Theme",
				"Change color theme of The Debuginator.", debuginator__on_change_theme, debuginator,
				string_titles, (void*)theme_indices, 4, sizeof(theme_indices[0]));
		}
	}
}

void debuginator_print(DebuginatorItem* item, int indentation) {
	DEBUGINATOR_debug_print("%*s%s\n", indentation, "", item->title);
	if (item->is_folder) {
		item = item->folder.first_child;
		while (item) {
			debuginator_print(item, indentation + 4);
			item = item->next_sibling;
		}
	}
	else {
		for (size_t i = 0; i < item->leaf.num_values; i++) {
			DEBUGINATOR_debug_print("%*s%s\n", indentation + 4, "", item->leaf.value_titles[i]);
		}
	}
}

DebuginatorItem* debuginator__find_first_leaf(DebuginatorItem* item) {
	if (!item->is_folder) {
		return item;
	}

	if (item->is_folder && item->folder.first_child != NULL) {
		DebuginatorItem* child = item->folder.first_child;
		while (child) {
			if (child->is_folder) {
				DebuginatorItem* leaf_item = debuginator__find_first_leaf(child);
				if (leaf_item != NULL) {
					return leaf_item;
				}

				child = child->next_sibling;
			}
			else {
				return child;
			}
		}
	}

	return NULL;
}

DebuginatorItem* debuginator__find_last_leaf(DebuginatorItem* item) {
	if (!item->is_folder) {
		return item;
	}

	if (item->is_folder && item->folder.first_child != NULL) {
		DebuginatorItem* child = item->folder.first_child;
		while (child->next_sibling) {
			child = child->next_sibling;
		}

		while (child) {
			if (child->is_folder) {
				DebuginatorItem* leaf_item = debuginator__find_last_leaf(child);
				if (leaf_item != NULL) {
					return leaf_item;
				}

				child = child->prev_sibling;
			}
			else {
				return child;
			}
		}
	}

	return NULL;
}

//██╗   ██╗██████╗ ██████╗  █████╗ ████████╗███████╗
//██║   ██║██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝██╔════╝
//██║   ██║██████╔╝██║  ██║███████║   ██║   █████╗
//██║   ██║██╔═══╝ ██║  ██║██╔══██║   ██║   ██╔══╝
//╚██████╔╝██║     ██████╔╝██║  ██║   ██║   ███████╗
//╚═════╝ ╚═╝     ╚═════╝ ╚═╝  ╚═╝   ╚═╝   ╚══════╝

bool debuginator__distance_to_hot_item(DebuginatorItem* item, DebuginatorItem* hot_item, float* distance) {
	if (item == hot_item) {
		if (!item->is_folder && item->leaf.is_active) {
			*distance += 30 * (item->leaf.hot_index + 1);
		}
		return true;
	}

	*distance += 30;
	if (item->is_folder) {
		DebuginatorItem* child = item->folder.first_child;
		while (child) {
			bool found = debuginator__distance_to_hot_item(child, hot_item, distance);
			if (found) {
				return true;
			}

			child = child->next_sibling;
		}
	}

	return false;
}

void debuginator_update(TheDebuginator* debuginator, float dt) {
	debuginator->dt = dt;
	debuginator->draw_timer += dt;
	if (debuginator->is_open && debuginator->openness < 1) {
		debuginator->openness_timer += dt;
		if (debuginator->openness_timer > 1) {
			debuginator->openness_timer = 1;
		}

		debuginator->openness = debuginator__ease_out(debuginator->openness_timer, 0, 1, 1);
	}

	else if (!debuginator->is_open && debuginator->openness > 0) {
		debuginator->openness_timer -= dt;
		if (debuginator->openness_timer < 0) {
			debuginator->openness_timer = 0;
		}

		debuginator->openness = debuginator__ease_out(debuginator->openness_timer, 0, 1, 1);
	}


	// Ensure hot item is smoothly placed at a nice position
	float distance_from_root_to_hot_item = 0;
	debuginator__distance_to_hot_item(debuginator->root, debuginator->hot_item, &distance_from_root_to_hot_item);
	float wanted_y = debuginator->size.y * debuginator->focus_height;
	float distance_to_wanted_y = wanted_y - distance_from_root_to_hot_item;
	debuginator->current_height_offset = debuginator__lerp(debuginator->current_height_offset, distance_to_wanted_y, dt);
	if (DEBUGINATOR_fabs(debuginator->current_height_offset - distance_to_wanted_y) < 0.1f) {
		debuginator->current_height_offset = distance_to_wanted_y;
	}
}


//██████╗ ██████╗  █████╗ ██╗    ██╗
//██╔══██╗██╔══██╗██╔══██╗██║    ██║
//██║  ██║██████╔╝███████║██║ █╗ ██║
//██║  ██║██╔══██╗██╔══██║██║███╗██║
//██████╔╝██║  ██║██║  ██║╚███╔███╔╝
//╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝ ╚══╝╚══╝

float debuginator_draw_item(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2 offset, bool hot);

void debuginator_draw(TheDebuginator* debuginator, float dt) {
	// TODO return if fully closed

	// update theme opacity
	DebuginatorTheme* source_theme = &debuginator->themes[debuginator->theme_index];
	for (size_t i = 0; i < DEBUGINATOR_NumDrawTypes; i++) {
		debuginator->theme.colors[i].a = (unsigned char)(source_theme->colors[i].a * debuginator->openness);
	}

	float lerp_t = (float)(DEBUGINATOR_sin(debuginator->draw_timer * 2) + 1) * 0.5f;
	debuginator->theme.colors[DEBUGINATOR_ItemTitleActive].r = (unsigned char)debuginator__lerp((float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive1].r, (float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive2].r, lerp_t);
	debuginator->theme.colors[DEBUGINATOR_ItemTitleActive].g = (unsigned char)debuginator__lerp((float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive1].g, (float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive2].g, lerp_t);
	debuginator->theme.colors[DEBUGINATOR_ItemTitleActive].b = (unsigned char)debuginator__lerp((float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive1].b, (float)debuginator->theme.colors[DEBUGINATOR_ItemTitleActive2].b, lerp_t);



	// Background
	DebuginatorVector2 offset = debuginator->root_position;
	offset.x += debuginator__lerp(0, debuginator->open_direction * debuginator->size.x, debuginator->openness);
	debuginator->draw_rect(offset, debuginator->size, debuginator->theme.colors[DEBUGINATOR_Background], debuginator->draw_user_data);

	offset.y = debuginator->current_height_offset;
	// Draw all items
	if (!debuginator->left_aligned) {
		//offset.x += debuginator->size.x;
	}
	//debuginator_draw_item(debuginator, debuginator->root, offset, true);

	DebuginatorItem* item_to_draw = debuginator->root->folder.first_child;
	while (item_to_draw && offset.y < -30) {
		// We'll start to draw off-screen which we don't want.
		if (offset.y + item_to_draw->total_height < 0) {
			// Whole item is off-screen, skip to sibling
			offset.y += item_to_draw->total_height;
			item_to_draw = item_to_draw->next_sibling;
		}
		else if (item_to_draw->is_folder) {
			// Part of item is off-screen, find which child to draw
			offset.x += 20;
			offset.y += 30;
			item_to_draw = item_to_draw->folder.first_child;
		}
		else {
			DEBUGINATOR_assert(false);
		}
	}

	while (item_to_draw && offset.y < debuginator->size.y) {
		debuginator_draw_item(debuginator, item_to_draw, offset, debuginator->hot_item == item_to_draw);
		offset.y += item_to_draw->total_height;
		while (item_to_draw && item_to_draw->next_sibling == NULL) {
			offset.x -= 20;
			item_to_draw = item_to_draw->parent;
		}
		
		if (item_to_draw == NULL) {
			break;
		}

		item_to_draw = item_to_draw->next_sibling;
	}

	// Update animations
	int running_animations = debuginator->animation_count; // Can be cleverer I guess
	for (int i = 0; i < debuginator->animation_count; i++) {
		DebuginatorAnimation* animation = &debuginator->animations[i];
		if (animation->time == animation->duration) {
			--running_animations;
			continue;
		}

		animation->time += dt;
		if (animation->time > animation->duration) {
			animation->time = animation->duration;
		}
		if (animation->type == DEBUGINATOR_ItemActivate) {
			float distance_from_root_to_item = 0;
			debuginator__distance_to_hot_item(debuginator->root, animation->data.item_activate.item, &distance_from_root_to_item);
			
			DebuginatorVector2 start_position = animation->data.item_activate.start_pos;
			DebuginatorVector2 end_position;
			end_position.x = 300;
			end_position.y = distance_from_root_to_item + debuginator->current_height_offset - (animation->data.item_activate.item->leaf.hot_index + 1) * 30 - 30; // HACK! for description :(


			DebuginatorVector2 position;
			position.x = debuginator__ease_out(animation->time, start_position.x, end_position.x - start_position.x, animation->duration);
			position.y = debuginator__ease_out(animation->time, start_position.y, end_position.y - start_position.y, animation->duration);
			//position.y = distance_from_root_to_item + debuginator->current_height_offset;
			DebuginatorFont* font = &debuginator->theme.fonts[DEBUGINATOR_ItemTitle];
			const char* text = animation->data.item_activate.item->leaf.value_titles[animation->data.item_activate.value_index];
			debuginator->draw_text(text, &position, &debuginator->theme.colors[DEBUGINATOR_ItemTitleActive], font, debuginator->draw_user_data);

		}
	}

	if (running_animations == 0) {
		debuginator->animation_count = 0;
	}
}

float debuginator_draw_item(TheDebuginator* debuginator, DebuginatorItem* item, DebuginatorVector2 offset, bool hot) {
	//draw_rect_filled(gui, offset, debuginator__vector2(100, 30), debuginator__color(200, 100, 50, 200));
	/*
	if (!debuginator->hot_item->is_folder && debuginator->hot_item->leaf.is_active) {
	if (debuginator->hot_item == item) {
	for (size_t i = 0; i < item->leaf.num_values; i++) {
	draw_rect_filled(gui, debuginator__vector2(0, offset.y + (i+1) * 30), debuginator__vector2(3, 20), debuginator__color(0, 255, 0, 255));
	}
	}
	}
	else if (debuginator->hot_item->parent == item->parent) {
	draw_rect_filled(gui, debuginator__vector2(0, offset.y), debuginator__vector2(3, 20), debuginator__color(0, 255, 0, 255));
	}*/

	if (item->is_folder) {
		if (debuginator->hot_item == item) {
			debuginator->draw_rect(debuginator__vector2(0, offset.y - 5), debuginator__vector2(500, 30), debuginator->theme.colors[DEBUGINATOR_LineHighlight], debuginator->draw_user_data);
		}

		unsigned color_index = item == debuginator->hot_item ? DEBUGINATOR_ItemTitleActive : (hot ? DEBUGINATOR_ItemTitleHot : DEBUGINATOR_FolderTitle);
		debuginator->draw_text(item->title, &offset, &debuginator->theme.colors[color_index], &debuginator->theme.fonts[DEBUGINATOR_ItemTitle], debuginator->draw_user_data);
		offset.x += 20;
		DebuginatorItem* child = item->folder.first_child;
		while (child) {
			offset.y += 30;
			if (offset.y > debuginator->size.y) {
				break;
			}

			offset.y = debuginator_draw_item(debuginator, child, offset, debuginator->hot_item == child);
			child = child->next_sibling;
		}
	}
	else {
		if (debuginator->hot_item == item && (!item->leaf.is_active || item->leaf.num_values == 0)) {
			DebuginatorVector2 line_pos = debuginator__vector2(debuginator->openness * 500 - 500, offset.y - 5);
			if (!debuginator->left_aligned) {
				line_pos.x = debuginator->screen_resolution.x - debuginator->openness * debuginator->size.x;
			}
			debuginator->draw_rect(line_pos, debuginator__vector2(500, 30), debuginator->theme.colors[DEBUGINATOR_LineHighlight], debuginator->draw_user_data);
		}

		bool is_overriden = item->leaf.active_index != 0;
		unsigned default_color_index = is_overriden ? DEBUGINATOR_ItemTitleOverridden : DEBUGINATOR_ItemTitle;
		unsigned color_index = item == debuginator->hot_item && !item->leaf.is_active ? DEBUGINATOR_ItemTitleActive : (hot ? DEBUGINATOR_ItemTitleHot : default_color_index);
		DebuginatorFont* font = &debuginator->theme.fonts[DEBUGINATOR_ItemTitle];
		debuginator->draw_text(item->title, &offset, &debuginator->theme.colors[color_index], font, debuginator->draw_user_data);

		debuginator->edit_types[item->leaf.edit_type].quick_draw(debuginator, item, &offset);
		
		if (item->leaf.is_active) {
			offset.x += 20;

			char description_line[256];
			const char* description = item->leaf.description;
			float description_width = debuginator->size.x - 150 - offset.x;
			if (!debuginator->left_aligned) {
				description_width = debuginator->screen_resolution.x - offset.x;
			}

			float description_height = 0;
			while (description) {
				description = debuginator->word_wrap(description, debuginator->theme.fonts[DEBUGINATOR_ItemDescription], description_width, description_line, sizeof(description_line), debuginator->draw_user_data);

				offset.y += 30;
				description_height += 30;
				debuginator->draw_text(description_line, &offset, &debuginator->theme.colors[DEBUGINATOR_ItemDescription], &debuginator->theme.fonts[DEBUGINATOR_ItemDescription], debuginator->draw_user_data);
			}

			// Feels kinda ugly to do this here but... works for now.
			debuginator__set_total_height(item, 30 + description_height + 30 * (item->leaf.num_values));

			debuginator->edit_types[item->leaf.edit_type].expanded_draw(debuginator, item, &offset);

		}
	}

	return offset.y;
}




//██╗███╗   ██╗██████╗ ██╗   ██╗████████╗
//██║████╗  ██║██╔══██╗██║   ██║╚══██╔══╝
//██║██╔██╗ ██║██████╔╝██║   ██║   ██║
//██║██║╚██╗██║██╔═══╝ ██║   ██║   ██║
//██║██║ ╚████║██║     ╚██████╔╝   ██║
//╚═╝╚═╝  ╚═══╝╚═╝      ╚═════╝    ╚═╝

void debuginator_set_open(TheDebuginator* debuginator, bool is_open) {
	debuginator->is_open = is_open;
}

void debuginator_activate(TheDebuginator* debuginator, DebuginatorItem* item) {
	item->leaf.draw_t = 0;
	if (item->leaf.num_values == 0) {
		return;
	}

	item->leaf.active_index = item->leaf.hot_index;

	DebuginatorAnimation* animation = debuginator__get_free_animation(debuginator);
	if (animation != NULL) {
		animation->type = DEBUGINATOR_ItemActivate;
		animation->data.item_activate.item = item;
		animation->data.item_activate.value_index = item->leaf.hot_index;
		animation->duration = 0.5f;

		DebuginatorItem* parent = item->parent;
		while (parent) {
			animation->data.item_activate.start_pos.x += 20;
			parent = parent->parent;
		}

		float y_dist_to_root = 0;
		debuginator__distance_to_hot_item(debuginator->root, item, &y_dist_to_root);
		animation->data.item_activate.start_pos.y = y_dist_to_root + debuginator->current_height_offset;
	}

	if (item->leaf.on_item_changed_callback == NULL) {
		return;
	}

	void* value = ((char*)item->leaf.values) + item->leaf.hot_index * item->leaf.array_element_size;
	item->leaf.on_item_changed_callback(item, value, item->leaf.value_titles[item->leaf.hot_index]);
}

void debuginator_move_sibling_previous(TheDebuginator* debuginator) {
	// This is a bit stupid, consider changing to doubly linked list
	DebuginatorItem* hot_item = debuginator->hot_item;

	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		if (--hot_item->leaf.hot_index < 0) {
			hot_item->leaf.hot_index = hot_item->leaf.num_values - 1;
		}
	}
	else {
		DebuginatorItem* hot_item_new = debuginator->hot_item;
		DebuginatorItem* parent_child = hot_item_new->parent->folder.first_child;
		if (parent_child == hot_item_new) {
			while (parent_child) {
				hot_item_new = parent_child;
				parent_child = parent_child->next_sibling;
			}
		}
		else {
			while (parent_child) {
				if (parent_child->next_sibling == hot_item_new) {
					hot_item_new = parent_child;
					break;
				}

				parent_child = parent_child->next_sibling;
			}
		}

		if (hot_item != hot_item_new) {
			hot_item_new->parent->folder.hot_child = hot_item_new;
			debuginator->hot_item = hot_item_new;
		}
	}
}

void debuginator_move_sibling_next(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;

	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		if (++hot_item->leaf.hot_index == hot_item->leaf.num_values) {
			hot_item->leaf.hot_index = 0;
		}
	}
	else {
		DebuginatorItem* hot_item_new = debuginator->hot_item;
		if (hot_item->next_sibling != NULL) {
			hot_item_new = hot_item->next_sibling;
		}
		else {
			hot_item_new = hot_item->parent->folder.first_child;
		}

		if (hot_item != hot_item_new) {
			hot_item_new->parent->folder.hot_child = hot_item_new;
			debuginator->hot_item = hot_item_new;
		}
	}
}

void debuginator_move_to_next(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	DebuginatorItem* hot_item_new = debuginator->hot_item;
	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		if (++hot_item->leaf.hot_index == hot_item->leaf.num_values) {
			hot_item->leaf.hot_index = 0;
		}
	}
	else if (hot_item->next_sibling) {
		hot_item_new = hot_item->next_sibling;
	}
	else {
		DebuginatorItem* parent = hot_item->parent;
		while (parent != NULL) {
			if (parent->next_sibling != NULL) {
				hot_item_new = parent->next_sibling;
				break;
			}

			parent = parent->next_sibling;
		}

		if (parent == NULL) {
			hot_item_new = debuginator->root->folder.first_child;
		}
	}

	if (hot_item != hot_item_new) {
		hot_item_new->parent->folder.hot_child = hot_item_new;
		debuginator->hot_item = hot_item_new;
	}
}

void debuginator_move_to_next_leaf(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	DebuginatorItem* hot_item_new = debuginator->hot_item;
	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		if (++hot_item->leaf.hot_index == hot_item->leaf.num_values) {
			hot_item->leaf.hot_index = 0;
		}
	}
	else {
		/*
		MR
		  A
			A1
			A2
			  A3
			  A4
		  B
			B1
			B2
		*/

		DebuginatorItem* sibling = hot_item->next_sibling;
		DebuginatorItem* parent = hot_item->parent;
		hot_item_new = NULL;
		while (hot_item_new == NULL) {
			while (sibling != NULL) {
				DebuginatorItem* leaf_item = debuginator__find_first_leaf(sibling);
				if (leaf_item != NULL) {
					hot_item_new = leaf_item;
					break;
				}

				sibling = sibling->next_sibling;
			}

			if (hot_item_new != NULL) {
				break;
			}

			while (parent->next_sibling == NULL) {
				if (parent == debuginator->root) {
					DebuginatorItem* leaf_item = debuginator__find_first_leaf(parent);
					hot_item_new = leaf_item;
					break;
				}

				parent = parent->parent;
			}

			sibling = parent->next_sibling;
		}

		hot_item_new->parent->folder.hot_child = hot_item_new;
		debuginator->hot_item = hot_item_new;
	}
}


void debuginator_move_to_prev_leaf(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	DebuginatorItem* hot_item_new = debuginator->hot_item;
	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		if (--hot_item->leaf.hot_index < 0) {
			hot_item->leaf.hot_index = hot_item->leaf.num_values - 1;
		}
	}
	else {
		/*
		MR
		A
		A1
		A2
		A3
		A4
		B
		B1
		B2
		*/


		DebuginatorItem* sibling = hot_item->prev_sibling;
		DebuginatorItem* parent = hot_item->parent;
		hot_item_new = NULL;
		while (hot_item_new == NULL) {
			while (sibling != NULL) {
				DebuginatorItem* leaf_item = debuginator__find_last_leaf(sibling);
				if (leaf_item != NULL) {
					hot_item_new = leaf_item;
					break;
				}

				sibling = sibling->prev_sibling;
			}

			if (hot_item_new != NULL) {
				break;
			}

			while (parent->prev_sibling == NULL) {
				if (parent == debuginator->root) {
					DebuginatorItem* leaf_item = debuginator__find_last_leaf(parent);
					hot_item_new = leaf_item;
					break;
				}

				parent = parent->parent;
			}

			sibling = parent->prev_sibling;
		}

		hot_item_new->parent->folder.hot_child = hot_item_new;
		debuginator->hot_item = hot_item_new;
	}
}

void debuginator_move_to_child(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	DebuginatorItem* hot_item_new = debuginator->hot_item;

	if (!hot_item->is_folder) {
		if (hot_item->leaf.is_active) {
			debuginator_activate(debuginator, debuginator->hot_item);
		}
		else {
			hot_item->leaf.is_active = true;
			debuginator__set_total_height(hot_item, (float)30 * (hot_item->leaf.num_values) + 30); // for description, HACK! :(
		}
	}
	else {
		if (hot_item->folder.hot_child != NULL) {
			hot_item_new = hot_item->folder.hot_child;
		}
		else if (hot_item->folder.first_child != NULL) {
			hot_item_new = hot_item->folder.first_child;
			hot_item_new->parent->folder.hot_child = hot_item_new;
		}

		if (hot_item != hot_item_new) {
			hot_item_new->parent->folder.hot_child = hot_item_new;
			debuginator->hot_item = hot_item_new;
		}
	}
}

void debuginator_move_to_parent(TheDebuginator* debuginator) {
	DebuginatorItem* hot_item = debuginator->hot_item;
	DebuginatorItem* hot_item_new = debuginator->hot_item;
	if (!hot_item->is_folder && hot_item->leaf.is_active) {
		hot_item->leaf.is_active = false;
		debuginator__set_total_height(hot_item, 30);
	}
	else if (hot_item->parent != debuginator->root) {
		hot_item_new = debuginator->hot_item->parent;
	}

	if (hot_item != hot_item_new) {
		hot_item_new->parent->folder.hot_child = hot_item_new;
		debuginator->hot_item = hot_item_new;
	}
}

typedef struct DebuginatorInput {
	//bool activate;
	bool move_sibling_previous;
	bool move_sibling_next;
	bool move_to_parent;
	bool move_to_child;
} DebuginatorInput;

void debug_menu_handle_input(TheDebuginator* debuginator, DebuginatorInput* input) {
	if (input->move_sibling_next) {
		debuginator_move_sibling_next(debuginator);
	}

	if (input->move_to_child) {
		debuginator_move_to_child(debuginator);
	}

	if (input->move_to_parent) {
		debuginator_move_to_parent(debuginator);
	}

	// HACK
	//if (hot_item != hot_item_new) {
		//debuginator->hot_item = hot_item_new;
	//}

	//if (input->activate && hot_item->!is_folder) {
	//	debuginator_activate(debuginator->hot_item);
	//}
}

// ██╗   ██╗████████╗██╗██╗     ██╗████████╗██╗   ██╗
// ██║   ██║╚══██╔══╝██║██║     ██║╚══██╔══╝╚██╗ ██╔╝
// ██║   ██║   ██║   ██║██║     ██║   ██║    ╚████╔╝
// ██║   ██║   ██║   ██║██║     ██║   ██║     ╚██╔╝
// ╚██████╔╝   ██║   ██║███████╗██║   ██║      ██║
//  ╚═════╝    ╚═╝   ╚═╝╚══════╝╚═╝   ╚═╝      ╚═╝

void debuginator_copy_1byte(DebuginatorItem* item, void* value, const char* value_title) {
	(void)value_title;
	memcpy(item->user_data, value, 1);
}

void debuginator_create_bool_item(TheDebuginator* debuginator, const char* path, const char* description, void* user_data) {
	static bool bool_values[2] = { false, true };
	static const char* bool_titles[2] = { "False", "True" };
	DEBUGINATOR_assert(sizeof(bool_values[0]) == 1);
	DebuginatorItem* item = debuginator_create_array_item(debuginator, NULL, path,
		description, debuginator_copy_1byte, user_data,
		bool_titles, bool_values, 2, sizeof(bool_values[0]));
	item->leaf.edit_type = DEBUGINATOR_EditTypeBoolean;
}

#endif // DEBUGINATOR_IMPLEMENTATION
#endif // INCLUDE_THE_DEBUGINATOR_H


/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
