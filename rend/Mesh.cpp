#include "Mesh.h"
#include "rendmath.h"
#include "Exception.h"
#include "utils.h"

#include "glm/gtx/intersect.hpp"

namespace rend
{

Mesh::Mesh()
{ }

Mesh::Mesh(const sys::vector<unsigned char>& _data)
{
  load(_data);
}

Mesh::Mesh(int _type)
{
  load(_type);
}

void Mesh::load(int _type)
{
  if(_type == TRIANGLE_MESH)
  {
    m_positions.add(rend::vec3(0.0f, 0.5f, 0));
    m_positions.add(rend::vec3(-0.5f, -0.5f, 0));
    m_positions.add(rend::vec3(0.5f, -0.5f, 0));

    m_texcoords.add(rend::vec2(0.5f, 1));
    m_texcoords.add(rend::vec2(0, 0));
    m_texcoords.add(rend::vec2(1, 0));

    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
  }
  else if(_type == QUAD_MESH)
  {
    m_positions.add(rend::vec3(-0.5f, 0.5f, 0));
    m_positions.add(rend::vec3(-0.5f, -0.5f, 0));
    m_positions.add(rend::vec3(0.5f, -0.5f, 0));
    m_positions.add(rend::vec3(0.5f, -0.5f, 0));
    m_positions.add(rend::vec3(0.5f, 0.5f, 0));
    m_positions.add(rend::vec3(-0.5, 0.5f, 0));

    m_texcoords.add(rend::vec2(0, 1));
    m_texcoords.add(rend::vec2(0, 0));
    m_texcoords.add(rend::vec2(1, 0));
    m_texcoords.add(rend::vec2(1, 0));
    m_texcoords.add(rend::vec2(1, 1));
    m_texcoords.add(rend::vec2(0, 1));

    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
  }
  else if(_type == GUI_QUAD_MESH)
  {
    m_positions.add(rend::vec3(0, 0, 0));
    m_positions.add(rend::vec3(0, 1, 0));
    m_positions.add(rend::vec3(1, 1, 0));
    m_positions.add(rend::vec3(1, 1, 0));
    m_positions.add(rend::vec3(1, 0, 0));
    m_positions.add(rend::vec3(0, 0, 0));

    m_texcoords.add(rend::vec2(0, 1));
    m_texcoords.add(rend::vec2(0, 0));
    m_texcoords.add(rend::vec2(1, 0));
    m_texcoords.add(rend::vec2(1, 0));
    m_texcoords.add(rend::vec2(1, 1));
    m_texcoords.add(rend::vec2(0, 1));

    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
  }
  else if(_type == NDC_QUAD_MESH)
  {
    m_positions.add(rend::vec3(-1, 1, 0));
    m_positions.add(rend::vec3(-1, -1, 0));
    m_positions.add(rend::vec3(1, -1, 0));
    m_positions.add(rend::vec3(1, -1, 0));
    m_positions.add(rend::vec3(1, 1, 0));
    m_positions.add(rend::vec3(-1, 1, 0));

    m_texcoords.add(rend::vec2(0, 1));
    m_texcoords.add(rend::vec2(0, 0));
    m_texcoords.add(rend::vec2(1, 0));
    m_texcoords.add(rend::vec2(1, 0));
    m_texcoords.add(rend::vec2(1, 1));
    m_texcoords.add(rend::vec2(0, 1));

    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
    m_normals.add(rend::vec3(0, 0, 1));
  }
  else
  {
    throw Exception("Invalid mesh type specified");
  }
}

#define PARSE_F_LINE(I)                            \
  Vertex v##I;                                     \
                                                   \
  {                                                \
    int idx = 0;                                   \
    split_string(tokens[1 + I], '/', innertokens); \
                                                   \
    if(innertokens.size() < 1) break;              \
    idx = atoi(innertokens[0].unsafe_raw()) - 1;   \
    if(idx < 0 || idx >= positions.size()) break;  \
    v##I.position = positions[idx];                \
                                                   \
    if(innertokens.size() >= 2)                    \
    {                                              \
      idx = atoi(innertokens[1].unsafe_raw()) - 1; \
      if(idx >= 0 && idx < texcoords.size())       \
      {                                            \
        v##I.texcoord = texcoords[idx];            \
      }                                            \
    }                                              \
                                                   \
    if(innertokens.size() >= 3)                    \
    {                                              \
      idx = atoi(innertokens[2].unsafe_raw()) - 1; \
      if(idx >= 0 && idx < normals.size())         \
      {                                            \
        v##I.normal = normals[idx];                \
      }                                            \
    }                                              \
  }

void Mesh::load(const sys::vector<unsigned char>& _data)
{
  sys::string line;

  sys::vector<sys::string> tokens;
  sys::vector<vec3> positions;
  sys::vector<vec2> texcoords;
  sys::vector<vec3> normals;
  sys::vector<sys::string> innertokens;

  // Go over bounds by one. We will fake this as a '\n'
  for(size_t ci = 0; ci <= _data.size(); ++ci)
  {
    char c = '\n';

    // Only sample from array if it is within bounds
    if(ci < _data.size())
    {
      c = _data[ci];
    }

    if(c == '\r')
    {
      continue;
    }
    else if(c != '\n')
    {
      line += c;
      continue;
    }

    if(line.length() < 1) continue;
    split_string_whitespace(line, tokens);
    line = "";

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
      do
      {
        PARSE_F_LINE(0)
        PARSE_F_LINE(1)
        PARSE_F_LINE(2)

        Face face;
        face.a = v0;
        face.b = v1;
        face.c = v2;
        m_faces.push_back(face);

        m_positions.add(v0.position);
        m_positions.add(v1.position);
        m_positions.add(v2.position);

        m_texcoords.add(v0.texcoord);
        m_texcoords.add(v1.texcoord);
        m_texcoords.add(v2.texcoord);

        m_normals.add(v0.normal);
        m_normals.add(v1.normal);
        m_normals.add(v2.normal);
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
          m_faces.push_back(face);

          m_positions.add(v2.position);
          m_positions.add(v3.position);
          m_positions.add(v0.position);

          m_texcoords.add(v2.texcoord);
          m_texcoords.add(v3.texcoord);
          m_texcoords.add(v0.texcoord);

          m_normals.add(v2.normal);
          m_normals.add(v3.normal);
          m_normals.add(v0.normal);
        }
        while(0);
      }
    }
  }
}

sys::ptr<Buffer> Mesh::positions()
{
  return &m_positions;
}

sys::ptr<Buffer> Mesh::texcoords()
{
  return &m_texcoords;
}

sys::ptr<Buffer> Mesh::normals()
{
  return &m_normals;
}

bool Mesh::intersect(const Ray& _ray, const mat4& _model, float& _distance) const
{
  bool rtn = false;
  _distance = FLT_MAX;

  Ray ray(_ray);

  mat4 invmodel = inverse(_model);

  ray.origin = invmodel *
    rend::vec4(ray.origin, 1);

  ray.direction = invmodel *
    rend::vec4(ray.direction, 0);

  for(size_t fi = 0; fi < m_faces.size(); ++fi)
  {
    Face f = m_faces[fi];
    vec2 bary;
    float nd = 0;

    if(intersectRayTriangle(ray.origin, ray.direction,
      f.a.position, f.b.position, f.c.position, bary, nd) == true)
    {
      rtn = true;

      if(nd < _distance)
      {
        _distance = nd;
      }
    }
  }

  return rtn;
}

void Mesh::add(const Face& _face)
{
  m_faces.push_back(_face);
}

sys::vector<Face>::iterator Mesh::faces_begin()
{
  return m_faces.begin();
}

sys::vector<Face>::iterator Mesh::faces_end()
{
  return m_faces.end();
}

}

