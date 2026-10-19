/**
 * @file scene_parser.h
 *
 * @brief Contains functions for loading .scene files.
 *
 * Creates Entity, Component and Asset objects from ecs_types.h based on
 * definitions in a .scene file and push them into SceneData.
 *
 * This does not load data (e.g. vertex and image data) into the Asset
 * objects. That is handled in scene_loader.h.
 */

#pragma once

#include "define.h"
#include "ecs/ecs_manager.h"
#include "data/mg_string.h"

typedef enum TokenType {

  INVALID_TOKEN,

  TOKEN_INT_LIT,
  TOKEN_FLOAT_LIT,
  TOKEN_STRING_LIT,

  TOKEN_HASH,
  TOKEN_PERIOD,
  TOKEN_COMMA,
  TOKEN_SEMICOL,
  TOKEN_EQUALS,
  TOKEN_DOUBLE_QUOTES,
  TOKEN_OPEN_CURLY,
  TOKEN_CLOSE_CURLY,

  TOKEN_IDENTIFIER,

  TOKEN_SCENE,
  TOKEN_ASSETS,
  TOKEN_ENTITIES,

  TOKEN_ENTITY,

  TOKEN_TRANSFORM,

  TOKEN_C_CODE,
  TOKEN_C_MESH,
  TOKEN_C_MATERIAL,
  TOKEN_C_TEXTURE,
  TOKEN_C_CAMERA,
  TOKEN_C_BOX_COLLIDER_3D,
  TOKEN_C_BALL_COLLIDER_3D,

  TOKEN_A_MESH,
  TOKEN_A_TEXTURE,

  TOKEN_V_POSITION,
  TOKEN_V_SCALE,
  TOKEN_V_ROTATION,
  TOKEN_V_PATH,
  TOKEN_V_ASSET,
  TOKEN_V_FOV,
  TOKEN_V_NEAR_PLANE,
  TOKEN_V_FAR_PLANE,

  TOKEN_TYPE_MAX,

} TokenType;

/**
 * @struct Generic token type.
 */
typedef struct Token {
  TokenType type;
  void* val;
} Token;

/**
 * @struct Used for converting strings to TokenType.
 */
typedef struct TokenMap {
  TokenType type;
  const char *str;
  const char *as_str;
} TokenMap;

/**
 * @brief Reads in a .scene file, creates Entity, Component and Asset
 * objects and pushes them into scene data.
 *
 * DOES NOT LOAD ASSET DATA.
 *
 * @param data, A pointer to the scene data.
 * @param path, A path to a .scene file.
 * @return TRUE if the scene was opened successfully, otherwise FALSE.
 */
b8 scene_open_from_text(SceneData* data, const char* path);

/**
 * @brief Reads the content of a .scene file into a char buffer.
 *
 * @param path, A path to a .scene file.
 * @param content, A pointer to an unallocated char buffer.
 * @param length, A pointer to a u64 for the file length to be read into.
 * @return TRUE, If the file was read successfully, otherwise FALSE.
 */
b8 scene_read_text_file(const char* path, char* content, u64* length);

/**
 * @brief Separates a char buffer into tokens and pushed them onto a
 * dynamic array.
 *
 * @param file_string, A pointer to the char buffer.
 * @param length, The length of the char buffer.
 * @param token_darray, A pointer that was created using darray_create().
 * @return TRUE is the char buffer was tokenized successfullly, otherwise
 * FALSE.
 */
b8 scene_tokenize_text_file(char* file_string, u64 length,
			    Token* token_darray);

b8 scene_generate_token(char* string, Token* token_darray);

b8 scene_parse_tokens(Token* tokens);

b8 scene_parse_hash(Token* tokens, i32* index);

b8 scene_parse_mesh_asset(Token* tokens, i32* index);

b8 scene_parse_texture_asset(Token* tokens, i32* index);

b8 scene_parse_entity(Token* tokens, i32* index);

b8 scene_parse_transform(Token* tokens, i32* index,
			 Transform* out_transform);

Handle32 scene_parse_code_component(Token* tokens, i32* index);

Handle32 scene_parse_mesh_component(Token* tokens, i32* index);

Handle32 scene_parse_material_component(Token* tokens, i32* index);

Handle32 scene_parse_texture_component(Token* tokens, i32* index);

Handle32 scene_parse_camera_component(Token* tokens, i32* index);

Handle32 scene_parse_box_collider_3d_component(Token* tokens,
                                               i32* index);

Handle32 scene_parse_ball_collider_3d_component(Token* tokens,
                                                i32* index);

// UTIL

/**
 * @brief Parses a Vec3 from a Token array in the form { x.x, y.y, z.z }
 *
 * @param tokens, A pointer to the token array.
 * @param index, A pointer to an int containing the current index.
 * @param vec, A pointer to the Vec3 value to be filled.
 * @return TRUE if parsing was successful, otherwise FALSE.
 */
b8 scene_parse_vec3(Token* tokens, i32* index, Vec3* vec);
