#ifndef ENGINE_H
#define ENGINE_H

// The following includes are intentional for exposing the complete engine API.
// Although these headers are not used directly in this file, they are re-exported
// for convenience so that clients only need to include engine.h.
#include "allocator.h" // IWYU pragma: keep
#include "entity.h" // IWYU pragma: keep
#include "window.h" // IWYU pragma: keep
#include "resourceManager.h" // IWYU pragma: keep
#include "inputManager.h" // IWYU pragma: keep
#include "glrenderSystem.h" // IWYU pragma: keep
#include "defines.h" // IWYU pragma: keep
#include "font.h" // IWYU pragma: keep
#include "sprite.h" //IWYU pragma: keep

#ifdef DEBUG_ENABLED
#include "debug.h" // IWYU pragma: keep

#endif



#endif // ENGINE_H
