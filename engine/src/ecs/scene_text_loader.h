/**
 * @file scene_text_loader.h
 */

#pragma once

#include "define.h"

#include "ecs_types.h"

#include <stdio.h>
#include <string.h>

b8 parse_scene(SceneData* data, FILE* file);

static b8 parse_entity(Handle32 handle, char* name);

static b8 parse_mesh(Handle32 handle, char* name);

static b8 parse_material(Handle32 handle, char* name);

static b8 parse_texture(Handle32 handle, char* name);

static b8 get_drawable_handles();

static b8 get_tokens();
