#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<vec3> verts_;
	std::vector<std::vector<vec3> > faces_;
	std::vector<vec2> uvs_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	vec3 vert(int i);
	vec2 uv(int iface, int nvert);
	std::vector<int> face(int idx);
};

#endif //__MODEL_H__