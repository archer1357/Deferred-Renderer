
int mygl_Uniform(lua_State *L) {
  //
  GLint p;
  glGetIntegerv(GL_CURRENT_PROGRAM, &p);

  if(!p) {
    luaL_error(L,"No program in use.");
  }

  //
  const char *n=luaL_checkstring(L,1);
  GLuint loc=glGetUniformLocation(p,n);

  if(loc==-1) {
    // luaL_error(L,"%s not declared in program.",n);
    return 0;
  }

  //
  GLint size,type;
  glGetActiveUniformsiv(p,1,&loc,GL_UNIFORM_SIZE,&size);
  glGetActiveUniformsiv(p,1,&loc,GL_UNIFORM_TYPE,&type);

  //
  int length;

  if(type==GL_FLOAT || type==GL_INT) {
    length=1;
  } else if(type==GL_FLOAT_VEC2 || type==GL_INT_VEC2) {
    length=2;
  } else if(type==GL_FLOAT_VEC3 || type==GL_INT_VEC3) {
    length=3;
  } else if(type==GL_FLOAT_VEC4 || type==GL_INT_VEC4 || type==GL_FLOAT_MAT2) {
    length=4;
  } else if(type==GL_FLOAT_MAT3) {
    length=9;
  } else if(type==GL_FLOAT_MAT4) {
    length=16;
  } else {
    luaL_error(L,"Setter for %s's type not implemented.",n);
  }

  //
  int arraySize=length*size;

  if(type==GL_FLOAT || type==GL_FLOAT_VEC2 ||
     type==GL_FLOAT_VEC3 || type==GL_FLOAT_VEC4 ||
     type==GL_FLOAT_MAT2 || type==GL_FLOAT_MAT3 ||
     type==GL_FLOAT_MAT4) {
    float *data = new float[arraySize];

    if(const char *err=mylua_tableToStaticNumberArray(L, 2, arraySize, true, false, data)) {
      delete [] data;
      luaL_error(L,err);
    }

    if(type==GL_FLOAT) {
      glUniform1fv(loc,size,data);
    } else if(type==GL_FLOAT_VEC2) {
      glUniform2fv(loc,size,data);
    } else if(type==GL_FLOAT_VEC3) {
      glUniform3fv(loc,size,data);
    } else if(type==GL_FLOAT_VEC4) {
      glUniform4fv(loc,size,data);
    } else if(type==GL_FLOAT_MAT2) {
      glUniformMatrix2fv(loc,size,GL_TRUE,data);
    } else if(type==GL_FLOAT_MAT3) {
      glUniformMatrix3fv(loc,size,GL_TRUE,data);
    } else if(type==GL_FLOAT_MAT4) {
      glUniformMatrix4fv(loc,size,GL_TRUE,data);
    }

    delete [] data;
  } else if(type==GL_INT || type==GL_INT_VEC2 ||
            type==GL_INT_VEC3 || type==GL_INT_VEC4) {
    int *data = new int[arraySize];

    if(const char *err=mylua_tableToStaticNumberArray(L, 2, arraySize, true, true, data)) {
      delete [] data;
      luaL_error(L,err);
    }

    if(type==GL_INT) {
      glUniform1iv(loc,size,data);
    } else if(type==GL_INT_VEC2) {
      glUniform2iv(loc,size,data);
    } else if(type==GL_INT_VEC3) {
      glUniform3iv(loc,size,data);
    } else if(type==GL_INT_VEC4) {
      glUniform4iv(loc,size,data);
    }

    delete [] data;
  } else {
    luaL_error(L,"Setter for %s's type not implemented.",n);
  }

  //
  return 0;
}

int mygl_PrintUniformBlocks(lua_State *L) {

  GLuint p=mygl_checkGLuintUserData(L,1);

  //uniform blocks, fields
  int blockCount;
  glGetProgramiv(p,GL_ACTIVE_UNIFORM_BLOCKS,&blockCount);

  for(int i=0;i<blockCount;i++) {
    char blockName[256];
    glGetActiveUniformBlockName(p,i,256,0,blockName);

    GLint ubSize;
    glGetActiveUniformBlockiv(p,i,GL_UNIFORM_BLOCK_DATA_SIZE,&ubSize);

    //
    std::cout <<i << ": " << blockName << "(" << ubSize << ")"<< std::endl;

    //
    int fieldCount;
    glGetActiveUniformBlockiv(p,i,GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,&fieldCount);

    int *fieldIndices=new int[fieldCount];
    glGetActiveUniformBlockiv(p,i,GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,fieldIndices);

    //
    for(int j=0;j<fieldCount;j++) {
      GLuint fieldInd=fieldIndices[j];

      if(fieldInd==-1) {
        std::cout << " " << j << ": none\n";
        continue;
      }

      //
      char fieldName[256];
      glGetActiveUniformName(p,fieldInd,256,0,fieldName);

      //
      int fieldOffset,fieldType,fieldSize, fieldArrayStride, fieldMatrixStride;
      glGetActiveUniformsiv(p,1,&fieldInd,GL_UNIFORM_OFFSET,&fieldOffset);
      glGetActiveUniformsiv(p,1,&fieldInd,GL_UNIFORM_SIZE,&fieldSize);
      glGetActiveUniformsiv(p,1,&fieldInd,GL_UNIFORM_TYPE,&fieldType);
      glGetActiveUniformsiv(p,1,&fieldInd,GL_UNIFORM_ARRAY_STRIDE,&fieldArrayStride);
      glGetActiveUniformsiv(p,1,&fieldInd,GL_UNIFORM_MATRIX_STRIDE,&fieldMatrixStride);

      std::cout << " " << j << ": " << fieldName << std::endl;
      std::cout << "  offset: " << fieldOffset << std::endl;
      std::cout << "  type: " << fieldType << std::endl;
      std::cout << "  size: " << fieldSize << std::endl;
      std::cout << "  arraystride: " << fieldArrayStride << std::endl;
      std::cout << "  matrixstride: " << fieldMatrixStride << std::endl;
    }

    delete [] fieldIndices;
  }

  std::cout << std::endl;
  return 0;
}