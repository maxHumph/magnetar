#include "scene_parser.h"

#include "data/darray.h"
#include "core/log.h"
#include "core/mmemory.h"

#include <errno.h>
#include <stdint.h>
#include <stddef.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static SceneData* data = NULL_PTR;

/*
  static String* entity_idens = NULL_PTR;
  static String* mesh_asset_idens = NULL_PTR;
  static String* texture_asset_idens = NULL_PTR;
*/

static const TokenMap token_map[] = {


  {TOKEN_HASH, "#", "TOKEN_HASH"},
  {TOKEN_PERIOD, ".", "TOKEN_PERIOD"},
  {TOKEN_COMMA, ",", "TOKEN_COMMA"},
  {TOKEN_SEMICOL, ";", "TOKEN_SEMICOL"},
  {TOKEN_EQUALS, "=", "TOKEN_EQUALS"},
  {TOKEN_OPEN_CURLY, "{", "TOKEN_OPEN_CURLY"},
  {TOKEN_CLOSE_CURLY, "}", "TOKEN_CLOSE_CURLY"},


  {TOKEN_SCENE, "scene", "TOKEN_SCENE"},
  {TOKEN_ASSETS, "assets", "TOKEN_ASSETS"},
  {TOKEN_ENTITIES, "entities", "TOKEN_ENTITIES"},

  {TOKEN_ENTITY, "entity", "TOKEN_ENTITY"},

  {TOKEN_TRANSFORM, "transform", "TOKEN_TRANSFORM"},

  {TOKEN_C_CODE, "c_code", "TOKEN_C_CODE"},
  {TOKEN_C_MESH, "c_mesh", "TOKEN_C_MESH"},
  {TOKEN_C_MATERIAL, "c_material", "TOKEN_C_MATERIAL"},
  {TOKEN_C_TEXTURE, "c_texture", "TOKEN_C_TEXTURE"},
  {TOKEN_C_CAMERA, "c_camera", "TOKEN_C_CAMERA"},
  {TOKEN_C_BOX_COLLIDER_3D, "c_box_collider_3d",
   "TOKEN_C_BOX_COLLIDER_3D"},
  {TOKEN_C_BALL_COLLIDER_3D, "c_ball_collider_3d",
   "TOKEN_C_BALL_COLLIDER_3D"},

  {TOKEN_A_MESH, "a_mesh", "TOKEN_A_MESH"},
  {TOKEN_A_TEXTURE, "a_texture", "TOKEN_A_TEXTURE"},

  {TOKEN_V_POSITION, "position", "TOKEN_V_POSITION"},
  {TOKEN_V_SCALE, "scale", "TOKEN_V_SCALE"},
  {TOKEN_V_ROTATION, "rotation", "TOKEN_V_ROTATION"},
  {TOKEN_V_ASSET, "asset", "TOKEN_V_ASSET"},
  {TOKEN_V_PATH, "path", "TOKEN_V_PATH"},
  {TOKEN_V_FOV, "fov", "TOKEN_V_FOV"},
  {TOKEN_V_NEAR_PLANE, "near_plane", "TOKEN_V_NEAR_PLANE"},
  {TOKEN_V_FAR_PLANE, "far_plane", "TOKEN_V_FAR_PLANE"},
  
};

SceneIdentifiers* create_scene_identifiers() {
  SceneIdentifiers* ids = mallocate(sizeof(SceneIdentifiers),
				    MEMORY_TAG_SCENE);
  ids->entities = darray_create(String);
  ids->meshes = darray_create(String);
  ids->textures = darray_create(String);
  return ids;
}

void destroy_scene_identifiers(SceneIdentifiers* ids) {
  darray_destroy(ids->entities);
  darray_destroy(ids->meshes);
  darray_destroy(ids->textures);
  mfree(ids, sizeof(SceneIdentifiers), MEMORY_TAG_SCENE);
}

SceneParser* create_scene_parser(SceneData* data,
				 SceneIdentifiers* ids) {

  SceneParser* parser = mallocate(sizeof(SceneParser),
				  MEMORY_TAG_SCENE);
  parser->index = 0;
  parser->data = data;
  parser->ids = ids;
  parser->tokens = darray_create(Token);
  return parser;
}

void destroy_scene_parser(SceneParser* parser) {
  parser->data = NULL_PTR;
  destroy_token_darray_vals(parser->tokens);
  darray_destroy(parser->tokens);
  mfree(parser, sizeof(SceneParser), MEMORY_TAG_SCENE);
}

void destroy_token_darray_vals(Token* tokens) {
  for (u32 i = 0; i < darray_get_length(tokens); i++) {
    switch (tokens[i].type) {

    case TOKEN_IDENTIFIER:
      destroy_string((String)(tokens[i].val));
      break;

    case TOKEN_STRING_LIT:
      destroy_string((String)(tokens[i].val));
      break;

    case TOKEN_INT_LIT:
      mfree(tokens[i].val, sizeof(i64), MEMORY_TAG_SCENE);
      break;

    case TOKEN_FLOAT_LIT:
      mfree(tokens[i].val, sizeof(f64), MEMORY_TAG_SCENE);
      break;

    default:
      break;

    };
  }
}

b8 scene_open_from_text(SceneData* data, SceneIdentifiers* ids,
			StringView path) {

  // @TODO move SceneData creating out of this function
  data->entities = darray_create(Entity);

  data->drawable_handles = darray_create(Handle32);
  data->meshes = darray_create(CMesh);
  data->materials = darray_create(CMaterial);
  data->textures = darray_create(CCTexture);
  data->cameras = darray_create(CCamera);

  data->mesh_assets = darray_create(AMesh);
  data->texture_assets = darray_create(ATexture);
  

  SceneParser* parser = create_scene_parser(data, ids);

  String file_string = NULL_PTR;

  if (!scene_read_text_file(path, &file_string)) {
    MERROR_CORE("Scene parser failed to read: %s", path);
    return FALSE;
  }

  MTRACE_CORE("%s read.", path);

  if (!scene_tokenize_text_file(parser, file_string)) {
    MERROR_CORE("Failed to tokenize text file");
    return FALSE;
  }

  MTRACE_CORE("%s tokenized.", path);

  print_token_darray(parser->tokens);

  destroy_string(file_string);

  if (!scene_parse_tokens(parser)) {
    MERROR_CORE("Failed to parse tokens");
    return FALSE;
  }

  MTRACE_CORE("%s parsed.", path);

  destroy_scene_parser(parser);

  return TRUE;
}

b8 scene_read_text_file(StringView path, String* content) {
  FILE *file;
  file = fopen(path, "r");
  if (file == NULL) {
    printf("%d: %s\n", errno, strerror(errno));
    return FALSE;
  }

  fseek(file, 0, SEEK_END);
  u64 length = ftell(file);
  if (length == 0) {
    MERROR_CORE("File: %s doesn't contain any data.");
    return FALSE;
  }
  fseek(file, 0, SEEK_SET);
  *content = alloc_string(length);
  if (content) {
    fread(*content, sizeof(char), length, file);
  } else {
    printf("%d: %s", errno, strerror(errno));
    return FALSE;
  }
  fclose(file);


  return TRUE;
}

b8 scene_tokenize_text_file(SceneParser* parser,
			    StringView file_string) {


  for (u64 i = 0; i < string_get_length(file_string); i++) {
    char buffer[100] = "";
    u64 j = 0;

    // Single char symbols

    if (file_string[i] == '#') {
      darray_push(parser->tokens, ((Token){TOKEN_HASH, NULL_PTR}));
      MTRACE("Pushed Token: HASH");
    } else if (file_string[i] == '.') {
      darray_push(parser->tokens, ((Token){TOKEN_PERIOD, NULL_PTR}));
      MTRACE("Pushed Token: PERIOD");
    } else if (file_string[i] == ',') {
      darray_push(parser->tokens, ((Token){TOKEN_COMMA, NULL_PTR}));
      MTRACE("Pushed Token: COMMA");
    } else if (file_string[i] == ';') {
      darray_push(parser->tokens,
                  ((Token){TOKEN_SEMICOL, NULL_PTR}));
      MTRACE("Pushed Token: SEMICOL");
    } else if (file_string[i] == '=') {
      darray_push(parser->tokens,
		  ((Token){TOKEN_EQUALS, NULL_PTR}));
      MTRACE("Pushed Token: EQUALS");
    } else if (file_string[i] == '{') {
      darray_push(parser->tokens,
                  ((Token){TOKEN_OPEN_CURLY, NULL_PTR}));
      MTRACE("Pushed Token: LCURLY");
    } else if (file_string[i] == '}') {
      darray_push(parser->tokens,
                  ((Token){TOKEN_CLOSE_CURLY, NULL_PTR}));
      MTRACE("Pushed Token: RCURLY");
    
      // Keywords and identifiers

    } else if (isalpha(file_string[i]) || file_string[i] == '_') {
      buffer[j] = file_string[i];
      i++;
      j++;

      while (isalnum(file_string[i]) || file_string[i] == '_') {
        buffer[j] = file_string[i];
        i++;
        j++;
      }
      i--;

      buffer[j + 1] = '\0';

      if (!scene_generate_token(parser, buffer)) {
        MERROR_CORE("\"%s\" is an invalid token", buffer);
        return FALSE;
      }

      // sting lits

    } else if (file_string[i] == '"') {

      MDEBUG("StrLit");

      i++;

      while (file_string[i] != '"') { // @TODO: Add \" \\ and \n
        buffer[j] = file_string[i];
        i++;
        j++;
      }

      //i++;
      j++;
      buffer[j] = '\0';

      // @TODO: Free mem
      /*
	char* val = mallocate(sizeof(char) * j, MEMORY_TAG_STRING); 
	mcopy_memory(val, buffer, sizeof(char) * j);
      */

      String val = create_string(buffer);

      Token token = {
        .type = TOKEN_STRING_LIT,
        .val = (void*)val,
      };

      darray_push(parser->tokens, token);
      MTRACE("Pushed Token: STR LIT - %s", (String)(token.val));

      // int/float lits

    } else if (isdigit(file_string[i]) || file_string[i] == '-') {
      b8 is_int = TRUE;
      buffer[j] = file_string[i];
      i++;
      j++;

      while (isdigit(file_string[i]) || file_string[i] == '.') {
        if (file_string[i] == '.') {
          is_int = FALSE;
        }
        buffer[j] = file_string[i];
        i++;
        j++;
      }

      buffer[j + 1] = '\0';

      if (is_int) {
        char* endptr;
        i64 num = strtol(buffer, &endptr, 10);
        i64* val = mallocate(sizeof(i64), MEMORY_TAG_SCENE); // @TODO: Free mem
        mcopy_memory(val, &num, sizeof(i64));

        if (*endptr == '\0') {
          Token token = {
            .type = TOKEN_INT_LIT,
            .val = (void*)val,
          };
          darray_push(parser->tokens, token);
	  MTRACE("Pushed Token: INT LIT - %l", *(i64*)token.val);
        } else {
          MERROR_CORE("Failed to parse int lit");
          return FALSE;
        }
      } else {
        char* endptr;
        f64 num = strtod(buffer, &endptr);
        f64* val = mallocate(sizeof(f64), MEMORY_TAG_SCENE); // @TODO: Free mem
        mcopy_memory(val, &num, sizeof(f64));
	
        if (*endptr == '\0') {
          Token token = {
            .type = TOKEN_FLOAT_LIT,
            .val = (void*)val,
          };
	  darray_push(parser->tokens, token);
	  MTRACE("Pushed Token: FLOAT LIT - %f", *(f64*)token.val);
        } else {
          MERROR_CORE("Failed to parse float lit");
          return FALSE;
        }
      }
    }
  }

  Token end_token = {
    .type = TOKEN_END,
    .val = NULL_PTR,
  };

  darray_push(parser->tokens, end_token);

  return TRUE;
}

b8 scene_generate_token(SceneParser* parser, char* string) {
  for (u32 i = 0; i < sizeof(token_map) / sizeof(TokenMap); i++) {
    if (strcmp(string, token_map[i].str) == 0) {
      Token token = {

        .type = token_map[i].type,
        .val = NULL_PTR,
      };

      darray_push(parser->tokens, token);
      MTRACE("Pushed Token: keyword");
      return TRUE;
    }
  }

  if (isalpha(string[0])) {

    String val = create_string(string);

    Token token = {
      .type = TOKEN_IDENTIFIER,
      .val = (void*)val,
    };
    MTRACE("Pushed Token: Identifier - %s", (String)(token.val));
    darray_push(parser->tokens, token);

    return TRUE;
  }

  return FALSE;
}
 
b8 scene_parse_tokens(SceneParser* parser) {

  parser->index = 0;

  MTRACE_CORE("Tokens to parse: %u", darray_get_length(parser->tokens));


  while (parser->index < darray_get_length(parser->tokens)) {
  MDEBUG_CORE("Index: %u", parser->index);
    switch (parser->tokens[parser->index].type) {

    case INVALID_TOKEN:
      MERROR_CORE("Invalid token was found!");
      return FALSE;
      break;

    case TOKEN_HASH:
      if (!scene_parse_hash(parser)) {
        MERROR_CORE("Failed to parse hash statement");
        return FALSE;
      }
      break;

    case TOKEN_A_MESH:
      if (!scene_parse_mesh_asset(parser)) {
        MERROR_CORE("Failed to parse mesh asset");
        return FALSE;
      }
      break;

    case TOKEN_A_TEXTURE:
      if (!scene_parse_texture_asset(parser)) {
        MERROR_CORE("Failed to parse texture asset");
        return FALSE;
      }
      break;

    case TOKEN_ENTITY:
      if (!scene_parse_entity(parser)) {
        MERROR_CORE("Failed to parse entity defenition");
        return FALSE;
      }
      break;

    case TOKEN_END:
      return TRUE;
      break;

    default:
      MERROR_CORE("Syntax error");
      return FALSE;
      break;
    };
  }

  MWARN_CORE("Parser did not find a: TOKEN_END");
  return TRUE;
}

b8 scene_parse_hash(SceneParser* parser) {
  MWARN_CORE("scene_parse_hash() is unimplemented.");
  parser->index++;
  while (parser->tokens[parser->index].type != TOKEN_SEMICOL) {
    parser->index++;
  }
  parser->index++;
  return TRUE;
}

b8 scene_parse_mesh_asset(SceneParser* parser) {
  parser->index++;

  AMesh mesh = {};
  String name;
  
  if (parser->tokens[parser->index].type == TOKEN_IDENTIFIER) {
    name = create_string((String)parser->tokens[parser->index].val);
    parser->index++;
  } else {
    MERROR_CORE("Mesh Asset needs an identifier");
    return FALSE;
  }
  
  if (parser->tokens[parser->index].type == TOKEN_OPEN_CURLY) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return FALSE;
  }

  while (parser->tokens[parser->index].type != TOKEN_CLOSE_CURLY) {

    if (parser->tokens[parser->index].type == TOKEN_PERIOD) {
      parser->index++;
      if (parser->tokens[parser->index].type == TOKEN_V_PATH) {
        parser->index++;
        if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
          parser->index++;
          if (parser->tokens[parser->index].type == TOKEN_STRING_LIT) {
            mesh.model_path =
	      create_string((String)
			    (parser->tokens[parser->index].val));
            parser->index++;
            if (parser->tokens[parser->index].type ==
		TOKEN_CLOSE_CURLY) {
              break;
            } else if (parser->tokens[parser->index].type ==
		       TOKEN_COMMA) {
              parser->index++;
            } else {
              MERROR_CORE("Expected \",\" or \"}\"");
              return FALSE;
            }
          } else {
            MERROR_CORE("Expected string lit");
            return FALSE;
          }
        } else {
          MERROR_CORE("Expected assignment operator");
          return FALSE;
        }
      } else { // @TODO: Add other member values.
	MERROR_CORE("Expected member value");
	return FALSE;
      }
    }
    parser->index++;
  }
  parser->index++;

  if (parser->tokens[parser->index].type == TOKEN_SEMICOL) {
    parser->index++;
  } else {
    MTRACE("%u", parser->index);
    MERROR_CORE("Expected \";\"");
    return FALSE;
  }

  darray_push(parser->data->mesh_assets, mesh);

  darray_push(parser->ids->meshes, create_string(name));
  
  return TRUE;
}

b8 scene_parse_texture_asset(SceneParser* parser) {
  parser->index++;

  ATexture texture = {};
  String name;

  if (parser->tokens[parser->index].type == TOKEN_IDENTIFIER) {
    name = create_string((String)parser->tokens[parser->index].val);
    parser->index++;
  } else {
    MERROR_CORE("Texture Asset needs an identifier");
    return FALSE;
  }

  if (parser->tokens[parser->index].type == TOKEN_OPEN_CURLY) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return FALSE;
  }

  while (parser->tokens[parser->index].type != TOKEN_CLOSE_CURLY) {

    if (parser->tokens[parser->index].type == TOKEN_PERIOD) {
      parser->index++;
      if (parser->tokens[parser->index].type == TOKEN_V_PATH) {
        parser->index++;
        if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
          parser->index++;
          if (parser->tokens[parser->index].type == TOKEN_STRING_LIT) {
            texture.image_path =
	      create_string((String)
			    (parser->tokens[parser->index].val));
            parser->index++;
            if (parser->tokens[parser->index].type ==
		TOKEN_CLOSE_CURLY) {
              break;
            } else if (parser->tokens[parser->index].type == TOKEN_COMMA) {
              parser->index++;
            } else {
              MERROR_CORE("Expected \",\" or \"}\"");
              return FALSE;
            }
          } else {
            MERROR_CORE("Expected string lit");
            return FALSE;
          }
        } else {
          MERROR_CORE("Expected assignment operator");
          return FALSE;
        }
      } else {// @TODO: Add other member values.
	MERROR_CORE("Expected member value");
	return FALSE;
      }
    }
    parser->index++;
  }
  parser->index++;

  if (parser->tokens[parser->index].type == TOKEN_SEMICOL) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \";\"");
    return FALSE;
  }

  darray_push(parser->data->texture_assets, texture);

  darray_push(parser->ids->textures, create_string(name));


  return TRUE;
}

b8 scene_parse_entity(SceneParser* parser) {
  MTRACE_CORE("Parsing entity");
  parser->index++;

  Entity entity = {};
  String name;

  if (parser->tokens[parser->index].type == TOKEN_IDENTIFIER) {
    name = create_string((String)parser->tokens[parser->index].val);
    parser->index++;
  } else {
    MERROR_CORE("Entity declaration needs an identifier");
    return FALSE;
  }

  if (parser->tokens[parser->index].type == TOKEN_OPEN_CURLY) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return FALSE;
  }

  while (parser->tokens[parser->index].type != TOKEN_CLOSE_CURLY) {
    MDEBUG("Token type: %s, Index: %u",
	   token_type_as_string(parser->tokens[parser->index].type),
	   parser->index);
    switch (parser->tokens[parser->index].type) {

    case TOKEN_TRANSFORM: {
      Transform transform;
      MDEBUG_CORE("Index 1: %u", parser->index);
      if (scene_parse_transform(parser, &transform)) {
	entity.transform = transform;
      } else {
	MERROR_CORE("Failed to parse transform");
	return FALSE;
      }
      break;
    }

    case TOKEN_C_CODE: {
      if ((entity.components[COMPONENT_INDEX_CODE] =
           scene_parse_code_component(parser)) ==
          NULL_HANDLE_32) {
        MERROR_CORE("Failed to parse code component: "
		    "NULL_HANDLE_32");
        return FALSE;
      }
      break;
    }

    case TOKEN_C_MESH: {
      if ((entity.components[COMPONENT_INDEX_MESH] =
           scene_parse_mesh_component(parser)) ==
          NULL_HANDLE_32) {
        MERROR_CORE("Failed to parse mesh component: "
		    "NULL_HANDLE_32");
        return FALSE;
      }
      break;
    }

    case TOKEN_C_MATERIAL: {
      if ((entity.components[COMPONENT_INDEX_MATERIAL] =
           scene_parse_material_component(parser)) ==
          NULL_HANDLE_32) {
        MERROR_CORE("Failed to parse material component: "
		    "NULL_HANDLE_32");
        return FALSE;
      }
      break;
    }

    case TOKEN_C_CAMERA: {
      if ((entity.components[COMPONENT_INDEX_CAMERA] =
           scene_parse_camera_component(parser)) ==
          NULL_HANDLE_32) {
        MERROR_CORE("Failed to parse camera component: "
		    "NULL_HANDLE_32");
        return FALSE;
      }
      break;
    }

    case TOKEN_C_BOX_COLLIDER_3D: {
      if ((entity.components[COMPONENT_INDEX_BOX_COLLIDER_3D] =
           scene_parse_box_collider_3d_component(parser)) ==
          NULL_HANDLE_32) {
        MERROR_CORE("Failed to parse box collider 3D component: "
		    "NULL_HANDLE_32");
        return FALSE;
      }
      break;
    }

    case TOKEN_C_BALL_COLLIDER_3D: {
      if ((entity.components[COMPONENT_INDEX_BALL_COLLIDER_3D] =
           scene_parse_ball_collider_3d_component(parser)) ==
          NULL_HANDLE_32) {
        MERROR_CORE("Failed to parse ball collider 3D component: "
		    "NULL_HANDLE_32");
        return FALSE;
      }
      break;
    }

    case INVALID_TOKEN: {
    default:
      MERROR_CORE("Invalid token at index: %u (%s)", parser->index,
		  token_type_as_string(parser->tokens[parser->index].type));
      return FALSE;
      break;
    }

    };

    if (parser->tokens[parser->index].type == TOKEN_COMMA) {
      parser->index++;
    }
  }

  parser->index++;

  if (parser->tokens[parser->index].type == TOKEN_SEMICOL) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \";\"");
    return FALSE;
  }

  darray_push(parser->data->entities, entity);
  darray_push(parser->ids->entities, create_string(name)); 

  return TRUE;
}

b8 scene_parse_transform(SceneParser* parser,
			 Transform* out_transform) {
  parser->index++;

  Transform transform = {};

  if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \"=\"");
    return FALSE;
  }

  if (parser->tokens[parser->index].type == TOKEN_OPEN_CURLY) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return FALSE;
  }

  MDEBUG_CORE("Index 2: %u", parser->index);

  while (parser->tokens[parser->index].type != TOKEN_CLOSE_CURLY) {

    if (parser->tokens[parser->index].type == TOKEN_PERIOD) {
      parser->index++;
      switch (parser->tokens[parser->index].type) {
      case TOKEN_V_POSITION:
        parser->index++;
        if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
          parser->index++;
	  Vec3 pos;
	  if (scene_parse_vec3(parser, &pos)) {
	    transform.position = pos;
	  } else {
	    MERROR_CORE("Failed to parse .position: Expected Vec3.");
	    return FALSE;
	  }
        } else {
          MERROR_CORE("Expected: \"=\"");
          return FALSE;
        }
        break;

      case TOKEN_V_SCALE:
        parser->index++;
        if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
          parser->index++;
	  Vec3 scale;
	  if (scene_parse_vec3(parser, &scale)) {
	    transform.scale = scale;
	  } else {
	    MERROR_CORE("Failed to parse .scale: Expected Vec3.");
	    return FALSE;
	  }
        } else {
          MERROR_CORE("Expected: \"=\"");
          return FALSE;
        }
	break;

      case TOKEN_V_ROTATION:
        parser->index++;
        if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
          parser->index++;
	  Vec3 rotation;
	  if (scene_parse_vec3(parser, &rotation)) {
	    transform.rotation = quat_from_euler(rotation);
	  } else {
	    MERROR_CORE("Failed to parse .rotation: Expected Vec3.");
	    return FALSE;
	  }
        } else {
          MERROR_CORE("Expected: \"=\"");
          return FALSE;
        }
	break;

      default:
	MERROR_CORE("Unexpected token.");
	return FALSE;
      };
    }
    parser->index++;
    if (parser->tokens[parser->index].type == TOKEN_COMMA) {
      parser->index++;
    }
  }

  parser->index++;

  /*
  if (parser->tokens[parser->index].type == TOKEN_COMMA) {
    parser->index++;
  }
  */

  MDEBUG_CORE("Index after parse_transform: %u", parser->index);

  *out_transform = transform;

  return TRUE;
}

Handle32 scene_parse_code_component(SceneParser* parser) {
  parser->index++;

  MERROR_CORE("CCode is unimplemented");
  return NULL_HANDLE_32;
}

Handle32 scene_parse_mesh_component(SceneParser* parser) {
  parser->index++;

  CMesh mesh = {};

  if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \"=\"");
    return NULL_HANDLE_32;
  }

  if (parser->tokens[parser->index].type == TOKEN_OPEN_CURLY) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return NULL_HANDLE_32;
  }

  while (parser->tokens[parser->index].type != TOKEN_CLOSE_CURLY) {

    if (parser->tokens[parser->index].type == TOKEN_PERIOD) {
      parser->index++;
      switch (parser->tokens[parser->index].type) {
      case TOKEN_V_ASSET:
        parser->index++;
        if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
          parser->index++;
          if (parser->tokens[parser->index].type == TOKEN_IDENTIFIER) {
	    StringView asset_name =
	      (String)(parser->tokens[parser->index].val);
	    u64 asset_handle =
	      darray_get_str_index(parser->ids->meshes,
				   asset_name);
	    if (asset_handle == U64_MAX) {
	      MERROR_CORE("\"%s\" is an undefined asset", asset_name);
	      return NULL_HANDLE_32;
	    } else {
	      mesh.mesh_asset_handle = (Handle32)asset_handle;
	    }

          } else {
            MERROR_CORE("Expected indentifier");
            return NULL_HANDLE_32;
          }
        } else {
          MERROR_CORE("Expected: \"=\"");
          return NULL_HANDLE_32;
        }
        break;

      default:
	MERROR_CORE("Unexpected token");
	return NULL_HANDLE_32;
      };
    }
    parser->index++;
    if (parser->tokens[parser->index].type == TOKEN_COMMA) {
      parser->index++;
    }
  }
  parser->index++;

  if (parser->tokens[parser->index].type == TOKEN_COMMA) {
    parser->index++;
  }

  darray_push(parser->data->meshes, mesh);

  return (Handle32)darray_get_length(parser->data->meshes);
}

Handle32 scene_parse_material_component(SceneParser* parser) {
  parser->index++;

  CMaterial material = {};

  if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \"=\"");
    return NULL_HANDLE_32;
  }

  if (parser->tokens[parser->index].type == TOKEN_OPEN_CURLY) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return NULL_HANDLE_32;
  }

  while (parser->tokens[parser->index].type != TOKEN_CLOSE_CURLY) {

    if (parser->tokens[parser->index].type == TOKEN_PERIOD) {
      parser->index++;
      // Nothing yet
    } else if (parser->tokens[parser->index].type == TOKEN_C_TEXTURE) {
      if ((material.texture_handle =
	   scene_parse_texture_component(parser)) ==
	  NULL_HANDLE_32) {
	MERROR_CORE("Failed to parse texture component:"
		    "NULL_HANDLE_32");
	return NULL_HANDLE_32;
      }
    }
    if (parser->tokens[parser->index].type == TOKEN_COMMA) {
      parser->index++;
    }
  }
  parser->index++;

  if (parser->tokens[parser->index].type == TOKEN_COMMA) {
    parser->index++;
  }

  darray_push(parser->data->materials, material);

  return (Handle32)darray_get_length(parser->data->materials);
}

Handle32 scene_parse_texture_component(SceneParser* parser) {
  parser->index++;

  CCTexture texture = {};

  if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \"=\"");
    return NULL_HANDLE_32;
  }

  if (parser->tokens[parser->index].type == TOKEN_OPEN_CURLY) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return NULL_HANDLE_32;
  }

  while (parser->tokens[parser->index].type != TOKEN_CLOSE_CURLY) {

    if (parser->tokens[parser->index].type == TOKEN_PERIOD) {
      parser->index++;
      switch (parser->tokens[parser->index].type) {
      case TOKEN_V_ASSET:
        parser->index++;
        if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
          parser->index++;
          if (parser->tokens[parser->index].type == TOKEN_IDENTIFIER) {
	    StringView asset_name =
	      (String)(parser->tokens[parser->index].val);
	    u64 asset_handle =
	      darray_get_str_index(parser->ids->textures,
				   asset_name);
	    if (asset_handle == U64_MAX) {
	      MERROR_CORE("\"%s\" is an undefined asset", asset_name);
	      return NULL_HANDLE_32;
	    } else {
	      texture.texture_asset_handle = (Handle32)asset_handle;
	    }

          } else {
            MERROR_CORE("Expected indentifier");
            return NULL_HANDLE_32;
          }
        } else {
          MERROR_CORE("Expected: \"=\"");
          return NULL_HANDLE_32;
        }
        break;

      default:
	MERROR_CORE("Unexpected token");
	return NULL_HANDLE_32;
      };
    }
    parser->index++;
    if (parser->tokens[parser->index].type == TOKEN_COMMA) {
      parser->index++;
    }
  }
  parser->index++;

  if (parser->tokens[parser->index].type == TOKEN_COMMA) {
    parser->index++;
  }

  darray_push(parser->data->textures, texture);

  return (Handle32)darray_get_length(parser->data->textures);
}

Handle32 scene_parse_camera_component(SceneParser* parser) {
  parser->index++;

  CCamera camera = {};

  if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \"=\"");
    return NULL_HANDLE_32;
  }

  if (parser->tokens[parser->index].type == TOKEN_OPEN_CURLY) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return NULL_HANDLE_32;
  }

  while (parser->tokens[parser->index].type != TOKEN_CLOSE_CURLY) {

    if (parser->tokens[parser->index].type == TOKEN_PERIOD) {
      parser->index++;
      switch (parser->tokens[parser->index].type) {
      case TOKEN_V_FOV:
        parser->index++;
        if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
          parser->index++;
	  if (parser->tokens[parser->index].type == TOKEN_FLOAT_LIT) {
	    camera.fov = (f32)(*(f64*)(parser->tokens[parser->index].val));
	  } else {
	    MERROR_CORE("Expected TOKEN_FLOAT_LIT");
	    return NULL_HANDLE_32;
	  }
        } else {
          MERROR_CORE("Expected: \"=\"");
          return NULL_HANDLE_32;
        }
        break;

      case TOKEN_V_NEAR_PLANE:
        parser->index++;
        if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
          parser->index++;
	  if (parser->tokens[parser->index].type == TOKEN_FLOAT_LIT) {
	    camera.near_plane = (f32)(*(f64*)(parser->tokens[parser->index].val));
	  } else {
	    MERROR_CORE("Expected TOKEN_FLOAT_LIT");
	    return NULL_HANDLE_32;
	  }
        } else {
          MERROR_CORE("Expected: \"=\"");
          return NULL_HANDLE_32;
        }
        break;

      case TOKEN_V_FAR_PLANE:
        parser->index++;
        if (parser->tokens[parser->index].type == TOKEN_EQUALS) {
          parser->index++;
	  if (parser->tokens[parser->index].type == TOKEN_FLOAT_LIT) {
	    camera.far_plane = (f32)(*(f64*)(parser->tokens[parser->index].val));
	  } else {
	    MERROR_CORE("Expected TOKEN_FLOAT_LIT");
	    return NULL_HANDLE_32;
	  }
        } else {
          MERROR_CORE("Expected: \"=\"");
          return NULL_HANDLE_32;
        }
        break;

      default:
	MERROR_CORE("Unexpected token");
	return NULL_HANDLE_32;
      };
    }
    parser->index++;
    if (parser->tokens[parser->index].type == TOKEN_COMMA) {
      parser->index++;
    }
  }

  parser->index++;

  if (parser->tokens[parser->index].type == TOKEN_COMMA) {
    parser->index++;
  }

  darray_push(parser->data->cameras, camera);

  return (Handle32)darray_get_length(parser->data->cameras);
}

Handle32 scene_parse_box_collider_3d_component(SceneParser* parser) {
  return NULL_HANDLE_32;
}

Handle32 scene_parse_ball_collider_3d_component(SceneParser* parser) {
  return NULL_HANDLE_32;
}

b8 scene_parse_vec3(SceneParser* parser, Vec3* vec) {
  MDEBUG_CORE("Index 3: %u", parser->index);
  Vec3 temp;
  if (parser->tokens[parser->index].type == TOKEN_OPEN_CURLY) {
    parser->index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return FALSE;
  }

  for (u32 i = 0; i < 3; i++) {
    if (parser->tokens[parser->index].type == TOKEN_FLOAT_LIT) {
      temp.index[i] = (f32)(*(f64*)(parser->tokens[parser->index].val));
      parser->index++;
    } else {
      MERROR_CORE("Expected FLOAT LIT");
      return FALSE;
    }
    if (parser->tokens[parser->index].type == TOKEN_COMMA) {
      parser->index++;
    }
  }

  if (parser->tokens[parser->index].type == TOKEN_CLOSE_CURLY) {
    //parser->index++;
  } else {
    MERROR_CORE("Expected \"}\"");
    return FALSE;
  }

  *vec = temp;

  return TRUE;
}

StringView token_type_as_string(TokenType type) {
  for (u32 i = 0; i < (sizeof(token_map) / sizeof(TokenMap)); i++) {
    if (token_map[i].type  == type) {
      return token_map[i].as_str;
    }
  }
  return NULL_PTR;
}

void print_token_darray(Token* tokens) {
  for (u32 i = 0; i < darray_get_length(tokens); i++) {
    switch (tokens[i].type) {
    case TOKEN_IDENTIFIER:
      MDEBUG_CORE("Token (%u): TOKEN_IDENTIFIER : %s", i,
		  (StringView)tokens[i].val);
      break;

    case TOKEN_STRING_LIT:
      MDEBUG_CORE("Token (%u): TOKEN_STR_LIT : %s", i,
		  (StringView)tokens[i].val);
      break;

    case TOKEN_INT_LIT:
      MDEBUG_CORE("Token (%u): TOKEN_INT_LIT : %l", i,
		  *(i64*)tokens[i].val);
      break;
      
    case TOKEN_FLOAT_LIT:
      MDEBUG_CORE("Token (%u): TOKEN_FLOAT_LIT : %f", i,
		  *(f64*)tokens[i].val);
      break;

    default:
      MDEBUG_CORE("Token (%u): %s", i,
		  token_type_as_string(tokens[i].type));
      break;
      
    };
  }
}
