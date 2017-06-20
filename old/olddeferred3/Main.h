
#include "Math3d.h"
//#include <GL/glew.h>
#include "gl_core_3_3.h"
#include "Window.h"

#include "FileMon.h"

#include <iostream>
#include <algorithm>


#include "Deferred.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "GeometryManager.h"
#include "UniformManager.h"


#include "MouseSmooth.h"
#include "Camera.h"
#include "Keyframer.h"

#include <sstream>
#include "Timer.h"


#include "Font.h"


//for(int i=0;i<9;i++) {transform.normalMat[i]=glm::value_ptr(normalMat)[(i/3)*4+i%3];}
// glBindBufferBase(GL_UNIFORM_BUFFER,1,materialBuf);
//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Material), &material);
//glGenBuffers(1, &materialBuf);
//glBindBuffer(GL_UNIFORM_BUFFER, materialBuf);
//glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), 0, GL_DYNAMIC_DRAW);
//layout(std140,binding=0) uniform ub
