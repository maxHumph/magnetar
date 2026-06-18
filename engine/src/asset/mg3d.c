#include "mg3d.h"

#include "obj_loader.h"
#include "data_structures/darray.h"
#include "core/mmemory.h"

#define MG3D_BIN_SIG 0x4433474Du

static Vec3* vpos = NULL_PTR;
static Vec2* vtex = NULL_PTR;
static u32* ipos = NULL_PTR;
static u32* itex = NULL_PTR;
static const u32 FILE_SIG = MG3D_BIN_SIG;

b8 mg3d_load(const char* path, Vertex** vbuf, u32* vcount, u32** ibuf, u32* icount) {


  FILE* file = fopen(path, "rb");
  u32 signature;

  if (file == NULL_PTR) {
    MERROR_CORE("Failed to open file: %s", path);
    return FALSE;
  }

  fseek(file, 0, SEEK_SET);

  fread(&signature, sizeof(u32), 1, file);

  fread(vcount, sizeof(u32), 1, file);
  *vbuf = mallocate(*vcount * sizeof(Vertex), MEMORY_TAG_MODEL);
  if (*vbuf == NULL_PTR) {
    MERROR_CORE("Failed to allocate vertex buffer for: %s", path);
    return FALSE;
  }
  fread(*vbuf, sizeof(Vertex), *vcount, file);
  
  fread(icount, sizeof(u32), 1, file);
  *ibuf = mallocate(*icount * sizeof(u32), MEMORY_TAG_MODEL);
  if (*ibuf == NULL_PTR) {
    MERROR_CORE("Failed to allocate index buffer for: %s", path);
    return FALSE;
  }
  fread(*ibuf, sizeof(u32), *icount, file);
  


  if (signature != FILE_SIG) {
    MERROR_CORE("Not a valid .mg3d file: %s", path);
    return FALSE;
  }



  fclose(file);

  return TRUE;
}

b8 mg3d_from_obj(const char* obj_path, const char* mg3d_path) {

  if (!obj_load(obj_path, &vpos, &vtex, &ipos, &itex)) {
    MERROR_CORE("Failed to load model: %s", obj_path);
    return FALSE;
  }

  u64 index_count = darray_get_length(ipos);
  Vertex* vbuf_raw = darray_create(Vertex);

  for (u64 i = 0; i < index_count; i++) {
    darray_push(vbuf_raw, mg3d_build_vertex(i));
  }

  Vertex* vbuf = darray_create(Vertex);
  u32* ibuf = darray_create(u32);

  if (!mg3d_index_vertices(vbuf_raw, &vbuf, &ibuf)) {
    MERROR_CORE("MG3D: Failed to index vertices");
    return FALSE;
  }

  if (!mg3d_write_file(mg3d_path, vbuf, ibuf)) {
    MERROR_CORE("Failed to write mg3d file");
    return FALSE;
  }

  darray_destroy(vbuf_raw);
  darray_destroy(vbuf);
  darray_destroy(ibuf);
  darray_destroy(vpos);
  darray_destroy(vtex);
  darray_destroy(ipos);
  darray_destroy(itex);
  vpos = NULL_PTR;
  vtex = NULL_PTR;
  ipos = NULL_PTR;
  itex = NULL_PTR;

  return TRUE;
}

static Vertex mg3d_build_vertex(u32 index) {
  Vertex v = {
    .position = vpos[ipos[index] - 1],
    .colour = {.r = 1.0f, .g = 1.0f, .b = 1.0f},
    .texture_coord = vtex[itex[index] - 1],
  };

  return v;
}

static b8 mg3d_index_vertices(Vertex* src_vbuf, Vertex** dst_vbuf, u32** dst_ibuf) {

  *dst_vbuf = darray_create(Vertex);
  *dst_ibuf = darray_create(u32);
  u64 index_count = darray_get_length(src_vbuf);

  for (u64 i = 0; i < index_count; i++) {
    darray_push(*dst_vbuf, src_vbuf[i]);
    darray_push(*dst_ibuf, i);
  }

  return TRUE;
}

static b8 mg3d_write_file(const char* out_path, Vertex* vbuf, u32* ibuf) {

  u32 vertex_count = darray_get_length(vbuf);
  u32 index_count = darray_get_length(ibuf);

  FILE* file = fopen(out_path, "wb");

  if (file == NULL_PTR) {
    MERROR_CORE("Failed to open file: %s", out_path);
    return FALSE;
  }

  fseek(file, 0, SEEK_SET);
  fwrite(&FILE_SIG, sizeof(u32), 1, file);
  fwrite(&vertex_count, sizeof(u32), 1, file);
  fwrite(vbuf, sizeof(Vertex), vertex_count, file);
  fwrite(&index_count, sizeof(u32), 1, file);
  fwrite(ibuf, sizeof(u32), index_count, file);

  fclose(file);

  return TRUE;
}
