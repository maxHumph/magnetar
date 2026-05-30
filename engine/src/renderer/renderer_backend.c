#include "renderer_backend.h"

#include "core/mmemory.h"
#include "define.h"
#include "renderer/vulkan/vulkan_backend.h"

b8 renderer_backend_create(RendererAPI api, const char* application_name,
                           RendererBackend* backend) {
  switch (api) {
    case RENDERER_API_VULKAN:
      backend->init = vulkan_backend_init;
      backend->shutdown = vulkan_backend_shutdown;
      backend->start_frame = vulkan_backend_start_frame;
      backend->end_frame = vulkan_backend_end_frame;
      backend->resized = vulkan_backend_resized;
      return TRUE;
  }
}

void renderer_backend_destroy(RendererBackend* backend) {
  backend->init = NULL_PTR;
  backend->shutdown = NULL_PTR;
  backend->start_frame = NULL_PTR;
  backend->end_frame = NULL_PTR;
  backend->resized = NULL_PTR;
}
