#include "renderer/renderer_frontend.h"

#include "core/log.h"
#include "core/mmemory.h"
#include "define.h"
#include "game_interface.h"
#include "renderer/renderer_backend.h"

static RendererBackend* renderer_backend = NULL_PTR;

b8 renderer_init(Game* game_instance, PlatformState* platform_state) {
  renderer_backend = mallocate(sizeof(RendererBackend), MEMORY_TAG_RENDERER);

  if (!renderer_backend_create(game_instance->application_info.renderer_api,
                               game_instance->application_info.start_title, renderer_backend)) {
    MFATAL_CORE("Failed to create renderer backend");
    return FALSE;
  }

  if (!renderer_backend->init(renderer_backend, game_instance->application_info.start_title,
                              platform_state)) {
    MERROR_CORE("Failed to init renderer backend");
    return FALSE;
  }

  return TRUE;
}

void renderer_shutdown() {
  renderer_backend_destroy(renderer_backend);
  mfree(renderer_backend, sizeof(RendererBackend), MEMORY_TAG_RENDERER);
}

b8 renderer_start_frame(f64 delta_time) {
  if (!renderer_backend->start_frame(renderer_backend, delta_time)) {
    MERROR_CORE("Renderer backend failed to start frame");
    return FALSE;
  }
  return TRUE;
}

b8 renderer_end_frame(f64 delta_time) {
  if (!renderer_backend->end_frame(renderer_backend, delta_time)) {
    MERROR_CORE("Renderer backend failed to end frame");
    return FALSE;
  }
  return TRUE;
}

b8 renderer_draw_frame(/* RenderData */) { return TRUE; }
