#include "Model.h"
#include "Buffer.h"
#include "Texture.h"
#include "Exception.h"
#include "utils.h"

#include <sys/file.h>
#include <sys/memory.h>

namespace rend
{

struct TextureInfo
{
  Texture texture;
  sys::string path;
};

Model::Model()
{ }

Model::~Model()
{ }

Model::Model(const sys::string& _path)
{
  load(_path);
}

Model::Model(const sys::vector<unsigned char>& _data)
{
  load(_data);
}

static sys::string dirname(const sys::string& _path)
{
  for(int i = _path.length() - 1; i >= 0; --i)
  {
    if(_path[i] == '/' || _path[i] == '\\')
    {
      return _path.substring(0, i + 1);
    }
  }

  return "";
}

void Model::process_mtl(const sys::string& _path)
{
  sys::string dirPath(dirname(_path));
  sys::ptr<Material> material;
  sys::string line;
  sys::vector<sys::string> tokens;

  for(sys::file file(_path); file.read_line(line);)
  {
    if(line.length() < 1) continue;

    split_string_whitespace(line, tokens);

    // TODO: Might have a space in the name
    if(tokens[0] == "newmtl" && tokens.size() >= 2)
    {
      material = &*m_materials.emplace_back();
      material->name = tokens[1];
    }
    else if(tokens[0] == "Kd" && tokens.size() >= 4)
    {
      if(!material)
      {
        throw Exception("Diffuse color specified for unknown material");
      }

      vec3 d(
        atof(tokens[1].unsafe_raw()),
        atof(tokens[2].unsafe_raw()),
        atof(tokens[3].unsafe_raw()));

      material->diffuse.x = d.x;
      material->diffuse.y = d.y;
      material->diffuse.z = d.z;
    }
    else if(tokens[0] == "map_Kd" && tokens.size() >= 2)
    {
      if(!material)
      {
        throw Exception("Diffuse map specified for unknown material");
      }

      for(sys::linked_list<TextureInfo>::iterator it = m_textures.begin(); it != m_textures.end(); ++it)
      {
        if(it->path == tokens[1])
        {
          material->texture = &it->texture;
          break;
        }
      }

      if(!material->texture)
      {
        sys::linked_list<TextureInfo>::iterator it = m_textures.emplace_back();
        it->path = tokens[1];
        it->texture.load(dirPath + tokens[1]);
        material->texture = &it->texture;
      }
    }
  }
}

#define PARSE_F_LINE(I)                            \
  Vertex v##I;                                     \
                                                   \
  {                                                \
    int idx = 0;                                   \
    split_string(tokens[1 + I], '/', innerTokens); \
                                                   \
    if(innerTokens.size() < 1) break;              \
    idx = atoi(innerTokens[0].unsafe_raw()) - 1;   \
    if(idx < 0 || idx >= positions.size()) break;  \
    v##I.position = positions[idx];                \
                                                   \
    if(innerTokens.size() >= 2)                    \
    {                                              \
      idx = atoi(innerTokens[1].unsafe_raw()) - 1; \
      if(idx >= 0 && idx < texcoords.size())       \
      {                                            \
        v##I.texcoord = texcoords[idx];            \
      }                                            \
    }                                              \
                                                   \
    if(innerTokens.size() >= 3)                    \
    {                                              \
      idx = atoi(innerTokens[2].unsafe_raw()) - 1; \
      if(idx >= 0 && idx < normals.size())         \
      {                                            \
        v##I.normal = normals[idx];                \
      }                                            \
    }                                              \
  }

void Model::load(const sys::string& _path)
{
  sys::vector<sys::string> lines;
  sys::string line;

  for(sys::file file(_path); file.read_line(line);)
  {
    lines.push_back(line);
  }

  process_obj(lines, _path);
}

void Model::process_obj(const sys::vector<sys::string>& _lines, const sys::string& _path)
{
  m_parts.clear();
  m_materials.clear();
  m_textures.clear();

  sys::string line;
  sys::vector<sys::string> tokens;
  sys::vector<vec3> positions;
  sys::vector<vec2> texcoords;
  sys::vector<vec3> normals;
  sys::vector<sys::string> innerTokens;

  sys::ptr<Part> part;
  sys::ptr<MaterialGroup> mg;
  sys::ptr<Material> material;

  for(size_t li = 0; li < _lines.size(); ++li)
  {
    line = _lines[li];
    if(line.length() < 1) continue;

    split_string_whitespace(line, tokens);

    if(tokens[0] == "v" && tokens.size() >= 4)
    {
      vec3 p(
        atof(tokens[1].unsafe_raw()),
        atof(tokens[2].unsafe_raw()),
        atof(tokens[3].unsafe_raw()));

      positions.push_back(p);
    }
    else if(tokens[0] == "vt" && tokens.size() >= 3)
    {
      vec2 p(
        atof(tokens[1].unsafe_raw()),
        atof(tokens[2].unsafe_raw()));

      texcoords.push_back(p);
    }
    else if(tokens[0] == "vn" && tokens.size() >= 4)
    {
      vec3 n(
        atof(tokens[1].unsafe_raw()),
        atof(tokens[2].unsafe_raw()),
        atof(tokens[3].unsafe_raw()));

      normals.push_back(n);
    }
    else if(tokens[0] == "f" && tokens.size() >= 4)
    {
      if(!part)
      {
        part = &*m_parts.emplace_back();
        part->m_name = "Untitled";
      }

      if(!material)
      {
        material = &*m_materials.emplace_back();
        material->name = "Default";
      }

      if(!mg)
      {
        mg = &*part->m_mgs.emplace_back();
        mg->material = material;
      }

      do
      {
        PARSE_F_LINE(0)
        PARSE_F_LINE(1)
        PARSE_F_LINE(2)

        Face face;
        face.a = v0;
        face.b = v1;
        face.c = v2;
        mg->mesh.add(face);

        mg->mesh.positions()->add(v0.position);
        mg->mesh.positions()->add(v1.position);
        mg->mesh.positions()->add(v2.position);

        mg->mesh.texcoords()->add(v0.texcoord);
        mg->mesh.texcoords()->add(v1.texcoord);
        mg->mesh.texcoords()->add(v2.texcoord);

        mg->mesh.normals()->add(v0.normal);
        mg->mesh.normals()->add(v1.normal);
        mg->mesh.normals()->add(v2.normal);
      }
      while(0);

      if(tokens.size() >= 5)
      {
        do
        {
          PARSE_F_LINE(2)
          PARSE_F_LINE(3)
          PARSE_F_LINE(0)

          Face face;
          face.a = v2;
          face.b = v3;
          face.c = v0;
          mg->mesh.add(face);

          mg->mesh.positions()->add(v2.position);
          mg->mesh.positions()->add(v3.position);
          mg->mesh.positions()->add(v0.position);

          mg->mesh.texcoords()->add(v2.texcoord);
          mg->mesh.texcoords()->add(v3.texcoord);
          mg->mesh.texcoords()->add(v0.texcoord);

          mg->mesh.normals()->add(v2.normal);
          mg->mesh.normals()->add(v3.normal);
          mg->mesh.normals()->add(v0.normal);
        }
        while(0);
      }
    }
    else if(tokens[0] == "usemtl" && tokens.size() >= 2)
    {
      mg = NULL;
      material = NULL;

      for(sys::linked_list<Material>::iterator it = m_materials.begin(); it != m_materials.end(); ++it)
      {
        if(it->name == tokens[1])
        {
          material = &(*it);
          break;
        }
      }

      if(!material)
      {
        material = &*m_materials.emplace_back();
        material->name = tokens[1];
      }
    }
    else if(tokens[0] == "g" || tokens[0] == "o")
    {
      part = &*m_parts.emplace_back();
      part->m_name = "Untitled";
      mg = NULL;

      if(tokens.size() >= 2)
      {
        part->m_name = tokens[1];
      }
    }
    else if(tokens[0] == "mtllib" && tokens.size() >= 2)
    {
      if(_path == "") continue;

      sys::string dirPath(dirname(_path));
      process_mtl(dirPath + tokens[1]);
    }
  }

  /* Find sizes and centers */
  vec3 minpos(FLT_MAX, FLT_MAX, FLT_MAX);
  vec3 maxpos(-FLT_MAX, -FLT_MAX, -FLT_MAX);

  for(sys::linked_list<Part>::iterator pit = m_parts.begin();
    pit != m_parts.end(); ++pit)
  {
    vec3 partminpos(FLT_MAX, FLT_MAX, FLT_MAX);
    vec3 partmaxpos(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    sys::out("Part: " + pit->m_name + "\n");

    for(sys::linked_list<MaterialGroup>::iterator mit = pit->m_mgs.begin();
      mit != pit->m_mgs.end(); ++mit)
    {
      for(sys::vector<Face>::iterator fit = mit->mesh.faces_begin();
        fit != mit->mesh.faces_end(); ++fit)
      {
        partminpos = min(partminpos, fit->a.position);
        partminpos = min(partminpos, fit->b.position);
        partminpos = min(partminpos, fit->c.position);

        partmaxpos = max(partmaxpos, fit->a.position);
        partmaxpos = max(partmaxpos, fit->b.position);
        partmaxpos = max(partmaxpos, fit->c.position);
      }
    }

    minpos = min(minpos, partminpos);
    maxpos = max(maxpos, partmaxpos);

    pit->m_center = (partminpos + partmaxpos) / 2.0f;
    pit->m_size = partmaxpos - partminpos;

  }

  m_center = (minpos + maxpos) / 2.0f;
  m_size = maxpos - minpos;
  printf("Size: %f %f %f\n", m_size.x, m_size.y, m_size.z);
}

void Model::load(const sys::vector<unsigned char>& _data)
{
  sys::vector<sys::string> lines;
  split_string_eol(_data, lines);

  process_obj(lines);
}

sys::linked_list<Part>::iterator Model::parts_begin()
{
  return m_parts.begin();
}

sys::linked_list<Part>::iterator Model::parts_end()
{
  return m_parts.end();
}

sys::linked_list<Material>::iterator Model::materials_begin()
{
  return m_materials.begin();
}

sys::linked_list<Material>::iterator Model::materials_end()
{
  return m_materials.end();
}

Part::Part()
  : m_color(1, 1, 1, 1)
{ }

sys::linked_list<MaterialGroup>::iterator Part::mgs_begin()
{
  return m_mgs.begin();
}

sys::linked_list<MaterialGroup>::iterator Part::mgs_end()
{
  return m_mgs.end();
}

vec3 Part::center() const
{
  return m_center;
}

const sys::string& Part::name() const
{
  return m_name;
}

void Part::color(const vec4& _color)
{
  m_color = _color;
}

vec4 Part::color() const
{
  return m_color;
}

Material::Material()
  : name("Untitled")
  , diffuse(1, 1, 1, 1)
{ }

vec3 Model::size() const
{
  return m_size;
}

int Model::parts() const
{
  int rtn = 0;

  for(sys::linked_list<Part>::iterator it = m_parts.begin();
    it != m_parts.end(); ++it)
  {
    ++rtn;
  }

  return rtn;
}

}

