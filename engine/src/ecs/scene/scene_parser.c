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

static String* entity_idens = NULL_PTR;
static String* mesh_asset_idens = NULL_PTR;
static String* texture_asset_idens = NULL_PTR;

static const TokenMap token_map[] = {

  /*

    {TOKEN_HASH, "#", "TOKEN_HASH"},
    {TOKEN_PERIOD, ".", "TOKEN_PERIOD"},
    {TOKEN_COMMA, ",", "TOKEN_COMMA"},
    {TOKEN_SEMICOL, ";", "TOKEN_SEMICOL"},
    {TOKEN_OPEN_CURLY, "{", "TOKEN_OPEN_CURLY"},
    {TOKEN_CLOSE_CURLY, "}", "TOKEN_CLOSE_CURLY"},

  */

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
  {TOKEN_V_ASSET, "asset", "TOKEN_V_ASSET"}
  
};

b8 scene_open_from_text(SceneData* data, const char* path) {

  data = data;

  if (entity_idens != NULL_PTR) {
    entity_idens = darray_create(String);
  }

  if (mesh_asset_idens != NULL_PTR) {
    mesh_asset_idens = darray_create(String);
  }

  if (texture_asset_idens != NULL_PTR) {
    texture_asset_idens = darray_create(String);
  }


  Token* token_list = darray_create(Token);
  char* file_string = NULL_PTR;
  u64 file_length = 0;

  if (!scene_read_text_file(path, file_string, &file_length)) {
    MERROR_CORE("Scene parser failed to read: %s", path);
    return FALSE;
  }



  return TRUE;
}

b8 scene_read_text_file(const char *path, char* content, u64* length) {
  FILE *file;
  file = fopen(path, "r");
  if (file == NULL) {
    printf("%d: %s\n", errno, strerror(errno));
    return FALSE;
  }

  fseek(file, 0, SEEK_END);
  *length = ftell(file);
  fseek(file, 0, SEEK_SET);
  content = malloc(*length);
  if (content) {
    fread(content, sizeof(char), *length, file);
  } else {
    printf("%d: %s", errno, strerror(errno));
    return FALSE;
  }
  fclose(file);


  return TRUE;
}

b8 scene_tokenize_text_file(char* file_string, u64 length,
                            Token* token_darray) {


  for (u64 i = 0; i < length; i++) {
    char buffer[100] = "";
    u64 j = 0;

    // Single char symbols

    if (file_string[i] == '#') {
      darray_push(token_darray, ((Token){TOKEN_HASH, NULL_PTR}));
      i++;
    } else if (file_string[i] == '.') {
      darray_push(token_darray, ((Token){TOKEN_PERIOD, NULL_PTR}));
      i++;
    } else if (file_string[i] == ',') {
      darray_push(token_darray, ((Token){TOKEN_COMMA, NULL_PTR}));
      i++;
    } else if (file_string[i] == ';') {
      darray_push(token_darray,
                  ((Token){TOKEN_SEMICOL, NULL_PTR}));
      i++;
    } else if (file_string[i] == '"') {
      darray_push(token_darray,
                  ((Token){TOKEN_DOUBLE_QUOTES, NULL_PTR}));
      i++;
    } else if (file_string[i] == '{') {
      darray_push(token_darray,
                  ((Token){TOKEN_OPEN_CURLY, NULL_PTR}));
      i++;
    } else if (file_string[i] == '}') {
      darray_push(token_darray,
                  ((Token){TOKEN_CLOSE_CURLY, NULL_PTR}));
      i++;
    
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

      buffer[j + 1] = '\0';

      if (!scene_generate_token(buffer, token_darray)) {
        MERROR_CORE("\"%s\" is an invalid token", buffer);
        return FALSE;
      }

      // sting lits

    } else if (buffer[i] == '"') {

      i++;

      while (buffer[i] != '"') { // @TODO: Add \" \\ and \n
        buffer[j] = file_string[i];
        i++;
        j++;
      }

      i++;
      j++;
      buffer[j] = '\0';

      // @TODO: Free mem
      char* val = mallocate(sizeof(char) * j, MEMORY_TAG_STRING); 
      mcopy_memory(val, buffer, sizeof(char) * j);

      Token token = {
        .type = TOKEN_STRING_LIT,
        .val = (void*)val,
      };

      darray_push(token_darray, token);

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
          darray_push(token_darray, token);
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
        } else {
          MERROR_CORE("Failed to parse float lit");
          return FALSE;
        }
      }
    }



    return TRUE;
  }
}

b8 scene_generate_token(char* string, Token* token_darray) {
  for (u32 i = 0; i < sizeof(token_map) / sizeof(TokenMap); i++) {
    if (strcmp(string, token_map[i].str) == 0) {
      Token token = {

        .type = token_map[i].type,
        .val = NULL_PTR,
      };

      darray_push(token_darray, token);
      return TRUE;
    }
  }

  if (isalpha(string[0])) {

    String val = create_string(string);

    mcopy_memory(val, string, strlen(string + 1));

    Token token = {
      .type = TOKEN_IDENTIFIER,
      .val = (void*)val,
    };

    return TRUE;
  }

  return FALSE;
}
 
b8 scene_parse_tokens(Token* tokens) {
  for (i32 i = 0; i < darray_get_length(tokens); i++) {
    switch (tokens[i].type) {

    case INVALID_TOKEN:
      MERROR_CORE("Invalid token was found!");
      return FALSE;
      break;

    case TOKEN_HASH:
      if (!scene_parse_hash(tokens, &i)) {
        MERROR_CORE("Failed to parse hash statement");
        return FALSE;
      }
      break;

    case TOKEN_A_MESH:
      if (!scene_parse_mesh_asset(tokens, &i)) {
        MERROR_CORE("Failed to parse mesh asset");
        return FALSE;
      }
      break;

    case TOKEN_A_TEXTURE:
      if (!scene_parse_texture_asset(tokens, &i)) {
        MERROR_CORE("Failed to parse texture asset");
        return FALSE;
      }
      break;

    case TOKEN_ENTITY:
      if (!scene_parse_entity(tokens, &i)) {
        MERROR_CORE("Failed to parse entity defenition");
        return FALSE;
      }
      break;

    default:
      MERROR_CORE("Syntax error");
      return FALSE;
      break;
    };
  }
}

b8 scene_parse_hash(Token* tokens, i32* index) {
  *index++;
  while (tokens[*index].type != TOKEN_SEMICOL) {
    index++;
  }
  return TRUE;
}

b8 scene_parse_mesh_asset(Token* tokens, i32* index) {
  *index++;

  AMesh mesh = {};
  String name;
  
  if (tokens[*index].type == TOKEN_IDENTIFIER) {
    name = create_string((String)tokens[*index].val);
    *index++;
  } else {
    MERROR_CORE("Mesh Asset needs an identifier");
    return FALSE;
  }
  
  if (tokens[*index].type == TOKEN_OPEN_CURLY) {
    *index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return FALSE;
  }

  while (tokens[*index].type != TOKEN_CLOSE_CURLY) {

    if (tokens[*index].type == TOKEN_PERIOD) {
      *index++;
      if (tokens[*index].type == TOKEN_V_ASSET) {
        *index++;
        if (tokens[*index].type == TOKEN_EQUALS) {
          *index++;
          if (tokens[*index].type == TOKEN_STRING_LIT) {
            mesh.model_path = (String)(tokens[*index].val);
            *index++;
            if (tokens[*index].type == TOKEN_CLOSE_CURLY) {
              *index++;
              break;
            } else if (tokens[*index].type == TOKEN_COMMA) {
              *index++;
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
      } // @TODO: Add other member values.
    }


    *index++;
  }

  if (tokens[*index].type == TOKEN_SEMICOL) {
    *index++;
  } else {
    MERROR_CORE("Expected \";\"");
    return FALSE;
  }

  darray_push(data->mesh_assets, mesh);

  darray_push(mesh_asset_idens, name);
  
  return TRUE;
}

b8 scene_parse_texture_asset(Token* tokens, i32* index) {
  *index++;

  ATexture texture = {};
  String name;

  if (tokens[*index].type == TOKEN_IDENTIFIER) {
    name = create_string((String)tokens[*index].val);
    *index++;
  } else {
    MERROR_CORE("Texture Asset needs an identifier");
    return FALSE;
  }

  if (tokens[*index].type == TOKEN_OPEN_CURLY) {
    *index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return FALSE;
  }

  while (tokens[*index].type != TOKEN_CLOSE_CURLY) {

    if (tokens[*index].type == TOKEN_PERIOD) {
      *index++;
      if (tokens[*index].type == TOKEN_V_ASSET) {
        *index++;
        if (tokens[*index].type == TOKEN_EQUALS) {
          *index++;
          if (tokens[*index].type == TOKEN_STRING_LIT) {
            texture.image_path = (String)(tokens[*index].val);
            *index++;
            if (tokens[*index].type == TOKEN_CLOSE_CURLY) {
              *index++;
              break;
            } else if (tokens[*index].type == TOKEN_COMMA) {
              *index++;
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
      } // @TODO: Add other member values.
    }
    *index++;
  }

  if (tokens[*index].type == TOKEN_SEMICOL) {
    *index++;
  } else {
    MERROR_CORE("Expected \";\"");
    return FALSE;
  }

  darray_push(data->texture_assets, texture);

  darray_push(texture_asset_idens, name);


  return TRUE;
}

b8 scene_parse_entity(Token* tokens, i32* index) {
  *index++;

  Entity entity = {};
  String name;

  if (tokens[*index].type == TOKEN_IDENTIFIER) {
    *index++;
  } else {
    MERROR_CORE("Entity declaration needs an identifier");
    return FALSE;
  }

  if (tokens[*index].type == TOKEN_OPEN_CURLY) {
    *index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return FALSE;
  }

  while (tokens[*index].type != TOKEN_CLOSE_CURLY) {
    switch (tokens[*index].type) {

    case TOKEN_TRANSFORM:
      Transform transform;
      if (scene_parse_transform(tokens, index, &transform)) {
	entity.transform = transform;
      } else {
	MERROR_CORE("Failed to parse transform");
	return FALSE;
      }
      break;

    case TOKEN_C_CODE:
      if ((entity.components[COMPONENT_INDEX_CODE] =
           scene_parse_code_component(tokens, index)) ==
          NULL_HANDLE_32) {
        MERROR_CORE("Failed to parse code component: "
		    "NULL_HANDLE_32");
        return FALSE;
      }
      break;

    case TOKEN_C_MESH:
      if ((entity.components[COMPONENT_INDEX_MESH] =
           scene_parse_mesh_component(tokens, index)) ==
          NULL_HANDLE_32) {
        MERROR_CORE("Failed to parse mesh component: "
		    "NULL_HANDLE_32");
        return FALSE;
      }
      break;

    case TOKEN_C_MATERIAL:
      if ((entity.components[COMPONENT_INDEX_MATERIAL] =
           scene_parse_material_component(tokens, index)) ==
          NULL_HANDLE_32) {
        MERROR_CORE("Failed to parse material component: "
		    "NULL_HANDLE_32");
        return FALSE;
      }
      break;

    case TOKEN_C_CAMERA:
      if ((entity.components[COMPONENT_INDEX_CAMERA] =
           scene_parse_camera_component(tokens, index)) ==
          NULL_HANDLE_32) {
        MERROR_CORE("Failed to parse camera component: "
		    "NULL_HANDLE_32");
        return FALSE;
      }
      break;

    case TOKEN_C_BOX_COLLIDER_3D:
      if ((entity.components[COMPONENT_INDEX_BOX_COLLIDER_3D] =
           scene_parse_box_collider_3d_component(tokens, index)) ==
          NULL_HANDLE_32) {
        MERROR_CORE("Failed to parse box collider 3D component: "
		    "NULL_HANDLE_32");
        return FALSE;
      }
      break;

    case TOKEN_C_BALL_COLLIDER_3D:
      if ((entity.components[COMPONENT_INDEX_BALL_COLLIDER_3D] =
           scene_parse_ball_collider_3d_component(tokens, index)) ==
          NULL_HANDLE_32) {
        MERROR_CORE("Failed to parse ball collider 3D component: "
		    "NULL_HANDLE_32");
        return FALSE;
      }
      break;

    case INVALID_TOKEN:
    default:
      MERROR_CORE("Invalid token");
      return FALSE;
      break;

    };

    if (tokens[*index].type = TOKEN_COMMA) {
      *index++;
    }
  }

  if (tokens[*index].type == TOKEN_SEMICOL) {
    *index++;
  } else {
    MERROR_CORE("Expected \";\"");
    return FALSE;
  }

  darray_push(data->entities, entity);
  darray_push(entity_idens, name);

  return TRUE;
}

b8 scene_parse_transform(Token* tokens, i32* index,
			 Transform* out_transform) {
  *index++;

  Transform transform = {};

  if (tokens[*index].type == TOKEN_EQUALS) {
    *index++;
  } else {
    MERROR_CORE("Expected \"=\"");
    return FALSE;
  }

  if (tokens[*index].type == TOKEN_OPEN_CURLY) {
    *index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return FALSE;
  }

  while (tokens[*index].type != TOKEN_CLOSE_CURLY) {

    if (tokens[*index].type == TOKEN_PERIOD) {
      *index++;
      switch (tokens[*index].type) {
      case TOKEN_V_POSITION:
        *index++;
        if (tokens[*index].type == TOKEN_EQUALS) {
          *index++;
	  Vec3 pos;
	  if (scene_parse_vec3(tokens, index, &pos)) {
	    MERROR_CORE("Failed to parse .positoin: Expected Vec3.");
	    return FALSE;
	  } else {
	    transform.position = pos;
	  }
        } else {
          MERROR_CORE("Expected: \"=\"");
          return FALSE;
        }
        break;

      case TOKEN_V_SCALE:
        *index++;
        if (tokens[*index].type == TOKEN_EQUALS) {
          *index++;
	  Vec3 scale;
	  if (scene_parse_vec3(tokens, index, &scale)) {
	    MERROR_CORE("Failed to parse .scale: Expected Vec3.");
	    return FALSE;
	  } else {
	    transform.scale = scale;
	  }
        } else {
          MERROR_CORE("Expected: \"=\"");
          return FALSE;
        }
	break;

      case TOKEN_V_ROTATION:
        *index++;
        if (tokens[*index].type == TOKEN_EQUALS) {
          *index++;
	  Vec3 rotation;
	  if (scene_parse_vec3(tokens, index, &rotation)) {
	    MERROR_CORE("Failed to parse .rotation: Expected Vec3.");
	    return FALSE;
	  } else {
	    transform.rotation = quat_from_euler(rotation);
	  }
        } else {
          MERROR_CORE("Expected: \"=\"");
          return FALSE;
        }
	break;
      };
    }
    *index++;
    if (tokens[*index].type == TOKEN_COMMA) {
      *index++;
    }
  }

  if (tokens[*index].type == TOKEN_SEMICOL) {
    *index++;
  } else {
    MERROR_CORE("Expected \";\"");
    return FALSE;
  }

  *out_transform = transform;

  return TRUE;
}

Handle32 scene_parse_code_component(Token* tokens, i32* index) {
  *index++;

  MERROR_CORE("CCode is unimplemented");
  return NULL_HANDLE_32;
}

Handle32 scene_parse_mesh_component(Token* tokens, i32* index) {
  *index++;

  CMesh mesh = {};

  if (tokens[*index].type == TOKEN_EQUALS) {
    *index++;
  } else {
    MERROR_CORE("Expected \"=\"");
    return NULL_HANDLE_32;
  }

  if (tokens[*index].type == TOKEN_OPEN_CURLY) {
    *index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return NULL_HANDLE_32;
  }

  while (tokens[*index].type != TOKEN_CLOSE_CURLY) {

    if (tokens[*index].type == TOKEN_PERIOD) {
      *index++;
      switch (tokens[*index].type) {
      case TOKEN_V_ASSET:
        *index++;
        if (tokens[*index].type == TOKEN_EQUALS) {
          *index++;
          if (tokens[*index].type == TOKEN_IDENTIFIER) {
	    StringView asset_name = (String)(tokens[*index].val);
	    Handle32 asset_handle =
	      (Handle32)darray_get_str_index(mesh_asset_idens,
					     asset_name);
	    if (asset_handle == U64_MAX) {
	      MERROR_CORE("\"%s\" is an undefined asset", asset_name);
	      return NULL_HANDLE_32;
	    } else {
	      mesh.mesh_asset_handle = asset_handle;
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
      };
    }
    *index++;
    if (tokens[*index].type == TOKEN_COMMA) {
      *index++;
    }
  }

  if (tokens[*index].type == TOKEN_SEMICOL) {
    *index++;
  } else {
    MERROR_CORE("Expected \";\"");
    return FALSE;
  }

  darray_push(data->meshes, mesh);

  return NULL_HANDLE_32;
}

Handle32 scene_parse_material_component(Token* tokens, i32* index) {
  *index++;

  CMaterial material = {};

  if (tokens[*index].type == TOKEN_EQUALS) {
    *index++;
  } else {
    MERROR_CORE("Expected \"=\"");
    return NULL_HANDLE_32;
  }

  if (tokens[*index].type == TOKEN_OPEN_CURLY) {
    *index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return NULL_HANDLE_32;
  }

  while (tokens[*index].type != TOKEN_CLOSE_CURLY) {

    if (tokens[*index].type == TOKEN_PERIOD) {
      *index++;
      // Nothing yet
    } else if (tokens[*index].type == TOKEN_C_TEXTURE) {
      *index++;
      if ((material.texture_handle =
	   scene_parse_texture_component(tokens, index)) ==
	  NULL_HANDLE_32) {
	MERROR_CORE("Failed to parse texture component:"
		    "NULL_HANDLE_32");
	return NULL_HANDLE_32;
      }
    }
    if (tokens[*index].type == TOKEN_COMMA) {
      *index++;
    }
  }

  if (tokens[*index].type == TOKEN_SEMICOL) {
    *index++;
  } else {
    MERROR_CORE("Expected \";\"");
    return FALSE;
  }

  darray_push(data->materials, material);

  return NULL_HANDLE_32;
}

Handle32 scene_parse_texture_component(Token* tokens, i32* index) {
  *index++;

  CCTexture texture = {};

  if (tokens[*index].type == TOKEN_EQUALS) {
    *index++;
  } else {
    MERROR_CORE("Expected \"=\"");
    return NULL_HANDLE_32;
  }

  if (tokens[*index].type == TOKEN_OPEN_CURLY) {
    *index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return NULL_HANDLE_32;
  }

  while (tokens[*index].type != TOKEN_CLOSE_CURLY) {

    if (tokens[*index].type == TOKEN_PERIOD) {
      *index++;
      switch (tokens[*index].type) {
      case TOKEN_V_ASSET:
        *index++;
        if (tokens[*index].type == TOKEN_EQUALS) {
          *index++;
          if (tokens[*index].type == TOKEN_IDENTIFIER) {
	    StringView asset_name = (String)(tokens[*index].val);
	    Handle32 asset_handle =
	      (Handle32)darray_get_str_index(texture_asset_idens,
					     asset_name);
	    if (asset_handle == U64_MAX) {
	      MERROR_CORE("\"%s\" is an undefined asset", asset_name);
	      return NULL_HANDLE_32;
	    } else {
	      texture.texture_asset_handle = asset_handle;
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
      };
    }
    *index++;
    if (tokens[*index].type == TOKEN_COMMA) {
      *index++;
    }
  }

  if (tokens[*index].type == TOKEN_SEMICOL) {
    *index++;
  } else {
    MERROR_CORE("Expected \";\"");
    return FALSE;
  }

  darray_push(data->textures, texture);

  return NULL_HANDLE_32;
}

Handle32 scene_parse_camera_component(Token* tokens, i32* index) {
  *index++;

  CCamera camera = {};

  if (tokens[*index].type == TOKEN_EQUALS) {
    *index++;
  } else {
    MERROR_CORE("Expected \"=\"");
    return NULL_HANDLE_32;
  }

  if (tokens[*index].type == TOKEN_OPEN_CURLY) {
    *index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return NULL_HANDLE_32;
  }

  while (tokens[*index].type != TOKEN_CLOSE_CURLY) {

    if (tokens[*index].type == TOKEN_PERIOD) {
      *index++;
      switch (tokens[*index].type) {
      case TOKEN_V_FOV:
        *index++;
        if (tokens[*index].type == TOKEN_EQUALS) {
          *index++;
	  if (tokens[*index].type == TOKEN_FLOAT_LIT) {
	    camera.fov = (f32)(*(f64*)(tokens[*index].val));
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
        *index++;
        if (tokens[*index].type == TOKEN_EQUALS) {
          *index++;
	  if (tokens[*index].type == TOKEN_FLOAT_LIT) {
	    camera.near_plane = (f32)(*(f64*)(tokens[*index].val));
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
        *index++;
        if (tokens[*index].type == TOKEN_EQUALS) {
          *index++;
	  if (tokens[*index].type == TOKEN_FLOAT_LIT) {
	    camera.far_plane = (f32)(*(f64*)(tokens[*index].val));
	  } else {
	    MERROR_CORE("Expected TOKEN_FLOAT_LIT");
	    return NULL_HANDLE_32;
	  }
        } else {
          MERROR_CORE("Expected: \"=\"");
          return NULL_HANDLE_32;
        }
        break;
      };
    }
    *index++;
    if (tokens[*index].type == TOKEN_COMMA) {
      *index++;
    }
  }

  if (tokens[*index].type == TOKEN_SEMICOL) {
    *index++;
  } else {
    MERROR_CORE("Expected \";\"");
    return FALSE;
  }

  darray_push(data->cameras, camera);

  return NULL_HANDLE_32;
}

Handle32 scene_parse_box_collider_3d_component(Token* tokens,
                                               i32* index) {
  return NULL_HANDLE_32;
}

Handle32 scene_parse_ball_collider_3d_component(Token* tokens,
                                                i32* index) {
  return NULL_HANDLE_32;
}

b8 scene_parse_vec3(Token* tokens, i32* index, Vec3* vec) {
  Vec3 temp;
  if (tokens[*index].type == TOKEN_OPEN_CURLY) {
    *index++;
  } else {
    MERROR_CORE("Expected \"{\"");
    return FALSE;
  }

  for (u32 i = 0; i < 3; i++) {
    if (tokens[*index].type == TOKEN_FLOAT_LIT) {
      temp.index[i] = (f32)(*(f64*)(tokens[*index].val));
      *index++;
    } else {
      MERROR_CORE("Expected FLOAT LIT");
      return FALSE;
    }
    if (tokens[*index].type == TOKEN_COMMA) {
      *index++;
    }
  }

  if (tokens[*index].type == TOKEN_CLOSE_CURLY) {
    *index;
  } else {
    MERROR_CORE("Expected \"}\"");
    return FALSE;
  }

  *vec = temp;

  return TRUE;
}
