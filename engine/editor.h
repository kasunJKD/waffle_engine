#ifndef EDITOR_H
#define EDITOR_H

#include "engine.h"
#include "entity.h"
#include "inputManager.h"

struct Editor {
	SpriteManager *sp_mgr;
	Camera camera;
	bool active;

	void update_camera(Camera *camera);
	void activate_editor();
	void init_editor(SpriteManager *sp_mgr);
	void deinit_editor();
	void update_editor(InputManager *i, float dt);
	void draw_editor();
};

#endif //ENTITY_H
