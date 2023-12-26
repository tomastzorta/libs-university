#include "rendmath.h"

#include "sys/vector.h"
#include "sys/memory.h"

namespace rend
{

struct Part;

struct Transform : sys::base
{
  sys::ptr<Part> part;
  vec3 translation;
  vec3 rotation;
  vec3 scale;

  mat4 model() const;
};

struct Frame : sys::base
{
  sys::ptr<Transform> transform(Part& _part, bool _add);
  sys::ptr<Transform> transform(const Part& _part);

private:
  sys::vector<Transform> m_transforms;
};

struct Animation : sys::base
{
  Animation();

  void apply(Transform& _transform, float _frame);

  //mat4 model(const Part& _part, float _frame);
  //vec3 rotation(const Part& _part, float _frame);

  sys::ptr<Frame> frame(int _frame);
  size_t frames() const;

  void duplicate_frame(int _frame);
  void remove_frame(int _frame);
  void reset_frame(int _frame);
  void clear();

private:
  sys::vector<Frame> m_frames;

};

}

