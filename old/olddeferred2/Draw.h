#ifndef DRAW
#define DRAW

#include <stdint.h>
#include <GL/glew.h>

class GeometryDraw {
protected:
  GLenum mode;
  int first,count;
public:
  GeometryDraw(GLenum mode,int first,int count);
  virtual ~GeometryDraw();
  virtual void draw() = 0;
  void setCount(int count);
};

class GeometryDrawArrays : public GeometryDraw {
public:
  GeometryDrawArrays(GLenum mode, int first, int count);
  void draw();
};

class GeometryDrawElements : public GeometryDraw {
private:
  GLenum type;
public:
  GeometryDrawElements(GLenum mode, int first, int count, GLenum type);
  void draw();
};

#endif
