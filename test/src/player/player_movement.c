#include "player_movement.h"

#include <define.h>
#include <core/input.h>

void do_player_movement(Entity* player) {

  Vec3 forward = quat_rotate(player->transform.rotation,
			     (Vec3){0.0f, 0.0f, -1.0f});

  Vec3 right = quat_rotate(player->transform.rotation,
			   (Vec3){1.0f, 0.0f, 0.0f});

  if (key_down(KEY_W)) {
    player->transform.position = vec3_add(player->transform.position,
					  vec3_scale(forward, 0.1f));
  }

  if (key_down(KEY_S)) {
    player->transform.position = vec3_sub(player->transform.position,
					  vec3_scale(forward, 0.1f));
  }

  if (key_down(KEY_A)) {
    player->transform.position = vec3_sub(player->transform.position,
					  vec3_scale(right, 0.1f));
  }

  if (key_down(KEY_D)) {
    player->transform.position = vec3_add(player->transform.position,
					  vec3_scale(right, 0.1f));
  }

  if (key_down(KEY_Q)) {
    player->transform.position.y += 0.1f;
  }

  if (key_down(KEY_E)) {
    player->transform.position.y -= 0.1f;
  }

  if (key_down(KEY_LEFT_ARROW)) {
    player->transform.rotation =
      quat_mul(quat_from_euler((Vec3){0.0f, M_TO_RAD(1.5f), 0.0f}),
	       player->transform.rotation);
  }

  if (key_down(KEY_RIGHT_ARROW)) {
    player->transform.rotation =
      quat_mul(quat_from_euler((Vec3){0.0f, M_TO_RAD(-1.5f), 0.0f}),
	       player->transform.rotation);
  }

  if (key_down(KEY_UP_ARROW)) {
    player->transform.rotation =
      quat_mul(player->transform.rotation,
	       quat_from_euler((Vec3){M_TO_RAD(1.0f), 0.0f, 0.0f}));
  }

  if (key_down(KEY_DOWN_ARROW)) {
    player->transform.rotation =
      quat_mul(player->transform.rotation,
	       quat_from_euler((Vec3){M_TO_RAD(-1.0f), 0.0f, 0.0f}));
  }
}
