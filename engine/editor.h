#ifndef EDITOR_H
#define EDITOR_H

#include "entity.h"
#include "inputManager.h"

struct Editor {
	Camera camera;
	bool active;

	void update_camera(Camera *camera);
	void activate_editor();
	void init_editor();
	void deinit_editor();
	void update_editor(InputManager *i, float dt);
	void draw_editor();
};

#endif //ENTITY_H
