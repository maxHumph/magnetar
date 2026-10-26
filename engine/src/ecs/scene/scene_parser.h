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

  TOKEN_END,

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

typedef struct SceneIdentifiers {
  String* entities;
  String* meshes;
  String* textures;
} SceneIdentifiers;

SceneIdentifiers* create_scene_identifiers();

void destroy_scene_identifiers(SceneIdentifiers* ids);

typedef struct SceneParser {
  SceneData* data;
  SceneIdentifiers* ids;
  Token* tokens;
  u64 index;
} SceneParser;

SceneParser* create_scene_parser(SceneData* data,
				 SceneIdentifiers* ids);

void destroy_scene_parser(SceneParser* parser);

void destroy_token_darray_vals(Token* tokens);


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
b8 scene_open_from_text(SceneData* data, SceneIdentifiers** ids,
			StringView path);

/**
 * @brief Reads the content of a .scene file into a char buffer.
 *
 * @param path, A path to a .scene file.
 * @param content, An unallocated String.
 * @return TRUE, If the file was read successfully, otherwise FALSE.
 */
b8 scene_read_text_file(StringView path, String* content);

/**
 * @brief Separates a char buffer into tokens and pushed them onto a
 * dynamic array.
 *
 * @param parser, A pointer to the SceneParser object.
 * @param file_string, A pointer to the char buffer.
 * @return TRUE is the char buffer was tokenized successfullly,
 * otherwise FALSE.
 */
b8 scene_tokenize_text_file(SceneParser* parser,
			    StringView file_string);

b8 scene_generate_token(SceneParser* parser, char* string);

b8 scene_parse_tokens(SceneParser* parser);

b8 scene_parse_hash(SceneParser* parser);

b8 scene_parse_mesh_asset(SceneParser* parser);

b8 scene_parse_texture_asset(SceneParser* parser);

b8 scene_parse_entity(SceneParser* parser);

b8 scene_parse_transform(SceneParser* parser,
			 Transform* out_transform);

Handle32 scene_parse_code_component(SceneParser* parser);

Handle32 scene_parse_mesh_component(SceneParser* parser);

Handle32 scene_parse_material_component(SceneParser* parser);

Handle32 scene_parse_texture_component(SceneParser* parser);

Handle32 scene_parse_camera_component(SceneParser* parser);

Handle32 scene_parse_box_collider_3d_component(SceneParser* parser);

Handle32 scene_parse_ball_collider_3d_component(SceneParser* parser);

// UTIL

/**
 * @brief Parses a Vec3 from a Token array in the form { x.x, y.y, z.z }
 *
 * @param tokens, A pointer to the token array.
 * @param index, A pointer to an int containing the current index.
 * @param vec, A pointer to the Vec3 value to be filled.
 * @return TRUE if parsing was successful, otherwise FALSE.
 */
b8 scene_parse_vec3(SceneParser* parser, Vec3* vec);

StringView token_type_as_string(TokenType type);

void print_token_darray(Token* tokens);
