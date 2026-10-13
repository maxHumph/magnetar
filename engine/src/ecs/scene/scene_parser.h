/**
 * @file scene_parser.h
 */

#pragma once

#include "define.h"

typedef enum TokenType {

  INVALID_TOKEN,

  TOKEN_INT_LIT,
  TOKEN_FLOAT_LIT,
  TOKEN_STRING_LIT,

  TOKEN_HASH,
  TOKEN_PERIOD,
  TOKEN_COMMA,
  TOKEN_SEMICOL,
  TOKEN_DOUBLE_QUOTES,
  TOKEN_OPEN_CURLY,
  TOKEN_CLOSE_CURLY,

  TOKEN_IDENTIFIER,

  TOKEN_SCENE,
  TOKEN_ASSETS,
  TOKEN_ENTITIES,

  TOKEN_ENTITY,

  TOKEN_TRANSFORM,

  TOKEN_C_MESH,
  TOKEN_C_MATERIAL,
  TOKEN_C_TEXTURE,
  TOKEN_C_BOX_COLLIDER_3D,
  TOKEN_C_BALL_COLLIDER_3D,

  TOKEN_A_MESH,
  TOKEN_A_TEXTURE,

  TOKEN_V_POSITION,
  TOKEN_V_SCALE,
  TOKEN_V_ROTATION,
  TOKEN_V_ASSET,

  TOKEN_TYPE_MAX,

} TokenType;

typedef struct Token {
  TokenType type;
  void* val;
} Token;

typedef struct TokenMap {
  TokenType type;
  const char *str;
  const char *as_str;
} TokenMap;

b8 scene_open_from_text(const char* path);

b8 scene_read_text_file(const char* path, char* content, u64* length);

b8 scene_tokenize_text_file(char* file_string, u64 length,
			    Token* token_darray);

b8 scene_generate_token(char* string, Token* token_darray);
