#include "ShaderManager.h"
#include <iostream>
#include <fstream>

ShaderManager::ShaderManager() {
}

ShaderManager::~ShaderManager() {
}


GLuint ShaderManager::get(const std::string &vsFn,const std::string &gsFn,const std::string &fsFn) {
  Program *program=getProgram(vsFn,gsFn,fsFn);
  return program->obj;
}

bool ShaderManager::reloadSource(const std::string &fn) {
  std::list<std::string> vertexShaderDeps,geometryShaderDeps,fragmentShaderDeps;

  auto it=sources.find(fn);

  if(it==sources.end()) {
    return false;
  }

  //
  Source *source=it->second;

  vertexShaderDeps.insert(vertexShaderDeps.end(),source->vertexShaderDeps.begin(),source->vertexShaderDeps.end());
  geometryShaderDeps.insert(geometryShaderDeps.end(),source->geometryShaderDeps.begin(),source->geometryShaderDeps.end());
  fragmentShaderDeps.insert(fragmentShaderDeps.end(),source->fragmentShaderDeps.begin(),source->fragmentShaderDeps.end());

  //sources.erase(fn);
  //delete source;

  //
  std::set<std::string> checkSources;

  for(auto vertexShaderFn : vertexShaderDeps) {
    Shader *shader=vertexShaders.find(vertexShaderFn)->second;

    for(auto sourceFn : shader->sourceDeps) {
      auto sourceIt=sources.find(sourceFn);

      if(sourceIt!=sources.end()) {
        Source *source2=sourceIt->second;
        source2->vertexShaderDeps.erase(vertexShaderFn);
        checkSources.insert(sourceFn);
      }
    }
  }

  for(auto geometryShaderFn : geometryShaderDeps) {
    Shader *shader=geometryShaders.find(geometryShaderFn)->second;

    for(auto sourceFn : shader->sourceDeps) {
      auto sourceIt=sources.find(sourceFn);

      if(sourceIt!=sources.end()) {
        Source *source2=sourceIt->second;
        source2->geometryShaderDeps.erase(geometryShaderFn);
        checkSources.insert(sourceFn);
      }
    }
  }


  for(auto fragmentShaderFn : fragmentShaderDeps) {
    Shader *shader=fragmentShaders.find(fragmentShaderFn)->second;

    for(auto sourceFn : shader->sourceDeps) {
      auto sourceIt=sources.find(sourceFn);

      if(sourceIt!=sources.end()) {
        Source *source2=sourceIt->second;
        source2->fragmentShaderDeps.erase(fragmentShaderFn);
        checkSources.insert(sourceFn);
      }
    }
  }

  //
  source->srcs.clear();
  source->vertexShaderDeps.clear();
  source->geometryShaderDeps.clear();
  source->fragmentShaderDeps.clear();
  if(loadSource(source,fn)) {

    //v
    for(auto vertexShaderFn : vertexShaderDeps) {
      Shader *shader=getVertexShader(vertexShaderFn);

      if(loadShader(shader,GL_VERTEX_SHADER,vertexShaderFn)) {
        for(auto programKey : shader->programDeps) {
          Program *program=programs.find(programKey)->second;
          linkProgram(program,programKey);
          //std::cout << programKey << " relinked.\n";
        }
      }
    }

    //g
    for(auto geometryShaderFn : geometryShaderDeps) {
      Shader *shader=getGeometryShader(geometryShaderFn);

      if(loadShader(shader,GL_GEOMETRY_SHADER,geometryShaderFn)) {
        for(auto programKey : shader->programDeps) {
          Program *program=programs.find(programKey)->second;
          linkProgram(program,programKey);
          //std::cout << programKey << " relinked.\n";
        }
      }
    }

    //f
    for(auto fragmentShaderFn : fragmentShaderDeps) {
      Shader *shader=getFragmentShader(fragmentShaderFn);

      if(loadShader(shader,GL_FRAGMENT_SHADER,fragmentShaderFn)) {
        for(auto programKey : shader->programDeps) {
          Program *program=programs.find(programKey)->second;
          linkProgram(program,programKey);
          //std::cout << programFn << " relinked.\n";
        }
      }
    }
  }

  //remove orphaned sources
  for(auto sourceFn : checkSources) {
    Source *source2=getSource(sourceFn);

    if(source2->vertexShaderDeps.empty() &&
      source2->geometryShaderDeps.empty() &&
      source2->fragmentShaderDeps.empty()) {
      sources.erase(sourceFn);
      std::cout << sourceFn << " : ShaderSource : removed.\n";
      delete source2;
    }
  }

  return true;
}

bool ShaderManager::removeProgram(const std::string &vsFn,const std::string &gsFn,const std::string &fsFn) {

  std::string key=vsFn+ " & " + gsFn + " & " + fsFn;

  auto it=programs.find(key);

  if(it==programs.end()) {
    return false;
  }

  //
  Program *program=it->second;

  //
  //v
  for(auto vertexShaderFn : program->vertexShaderDeps) {
    Shader *vertexShader=getVertexShader(vertexShaderFn);
    vertexShader->programDeps.erase(key);

    if(vertexShader->programDeps.empty()) {
      for(auto sourceFn : vertexShader->sourceDeps) {
        Source *source=getSource(sourceFn);
        source->vertexShaderDeps.erase(vertexShaderFn);

        if(source->vertexShaderDeps.empty() &&
          source->geometryShaderDeps.empty() &&
          source->fragmentShaderDeps.empty()) {

          //
          sources.erase(sourceFn);
          delete source;
        }
      }

      //
      vertexShaders.erase(vertexShaderFn);
      glDetachShader(program->obj,vertexShader->obj);
      glDeleteShader(vertexShader->obj);
      delete vertexShader;
    }
  }

  //g
  for(auto geometryShaderFn : program->geometryShaderDeps) {
    Shader *geometryShader=getGeometryShader(geometryShaderFn);
    geometryShader->programDeps.erase(key);

    if(geometryShader->programDeps.empty()) {
      for(auto sourceFn : geometryShader->sourceDeps) {
        Source *source=getSource(sourceFn);
        source->geometryShaderDeps.erase(geometryShaderFn);

        if(source->vertexShaderDeps.empty() &&
          source->geometryShaderDeps.empty() &&
          source->fragmentShaderDeps.empty()) {

          //
          sources.erase(sourceFn);
          delete source;
        }
      }

      //
      geometryShaders.erase(geometryShaderFn);
      glDetachShader(program->obj,geometryShader->obj);
      glDeleteShader(geometryShader->obj);
      delete geometryShader;
    }
  }

  //f
  for(auto fragmentShaderFn : program->fragmentShaderDeps) {
    Shader *fragmentShader=getFragmentShader(fragmentShaderFn);
    fragmentShader->programDeps.erase(key);

    if(fragmentShader->programDeps.empty()) {
      for(auto sourceFn : fragmentShader->sourceDeps) {
        Source *source=getSource(sourceFn);
        source->fragmentShaderDeps.erase(fragmentShaderFn);

        if(source->vertexShaderDeps.empty() &&
          source->geometryShaderDeps.empty() &&
          source->fragmentShaderDeps.empty()) {

          //
          sources.erase(sourceFn);
          delete source;
        }
      }

      //
      fragmentShaders.erase(fragmentShaderFn);
      glDetachShader(program->obj,fragmentShader->obj);
      glDeleteShader(fragmentShader->obj);
      delete fragmentShader;
    }
  }

  //
  program->vertexShaderDeps.clear();
  program->geometryShaderDeps.clear();
  program->fragmentShaderDeps.clear();

  //
  programs.erase(it);
  glDeleteProgram(program->obj);
  delete program;

  //
  return true;
}

void ShaderManager::clear() {
  for(auto i=programs.begin();i!=programs.end();i++) {
    glDeleteProgram(i->second->obj);
    delete i->second;
  }

  for(auto i=vertexShaders.begin();i!=vertexShaders.end();i++) {
    glDeleteShader(i->second->obj);
    delete i->second;
  }

  for(auto i=geometryShaders.begin();i!=geometryShaders.end();i++) {
    glDeleteShader(i->second->obj);
    delete i->second;
  }

  for(auto i=fragmentShaders.begin();i!=fragmentShaders.end();i++) {
    glDeleteShader(i->second->obj);
    delete i->second;
  }

  for(auto i=sources.begin();i!=sources.end();i++) {
    delete i->second;
  }

  programs.clear();
  vertexShaders.clear();
  geometryShaders.clear();
  fragmentShaders.clear();
  sources.clear();
}

ShaderManager::Source *ShaderManager::getSource(const std::string &fn) {
  auto it=sources.find(fn);

  if(it!=sources.end()) {
    return it->second;
  }

  //
  //std::cout << "New Source " << fn << std::endl;

  //
  Source *source=new Source();
  sources.insert(std::make_pair(fn,source));

  loadSource(source,fn);

  return source;
}

ShaderManager::Shader *ShaderManager::getVertexShader(const std::string &fn) {
  auto it=vertexShaders.find(fn);

  if(it!=vertexShaders.end()) {
    return it->second;
  }

  //
  //std::cout << "New Vertex Shader " << fn << std::endl;

  //
  Shader *shader=new Shader();
  vertexShaders.insert(std::make_pair(fn,shader));
  shader->obj=glCreateShader(GL_VERTEX_SHADER);

  loadShader(shader,GL_VERTEX_SHADER,fn);

  return shader;
}

ShaderManager::Shader *ShaderManager::getGeometryShader(const std::string &fn) {
  auto it=geometryShaders.find(fn);

  if(it!=geometryShaders.end()) {
    return it->second;
  }

  //
  //std::cout << "New Geometry Shader " << fn << std::endl;

  //
  Shader *shader=new Shader();
  geometryShaders.insert(std::make_pair(fn,shader));
  shader->obj=glCreateShader(GL_GEOMETRY_SHADER);

  loadShader(shader,GL_GEOMETRY_SHADER,fn);

  return shader;
}

ShaderManager::Shader *ShaderManager::getFragmentShader(const std::string &fn) {
  auto it=fragmentShaders.find(fn);

  if(it!=fragmentShaders.end()) {
    return it->second;
  }

  //
  //std::cout << "New Fragment Shader " << fn << std::endl;

  //
  Shader *shader=new Shader();
  fragmentShaders.insert(std::make_pair(fn,shader));
  shader->obj=glCreateShader(GL_FRAGMENT_SHADER);

  loadShader(shader,GL_FRAGMENT_SHADER,fn);

  return shader;
}

ShaderManager::Program *ShaderManager::getProgram(const std::string &vsFn,const std::string &gsFn,const std::string &fsFn) {
  std::string key=vsFn+ " & " + gsFn + " & " + fsFn;

  auto it=programs.find(key);

  if(it!=programs.end()) {
    return it->second;
  }

  Program *program = new Program();
  programs.insert(std::make_pair(key,program));
  program->obj=glCreateProgram();

  //
  loadProgram(program,vsFn,gsFn,fsFn);
  return program;
}
bool ShaderManager::loadSource(Source *source,const std::string &fn) {
  source->ok=false;
  //source->srcs.clear();

  //
  std::ifstream file(fn.c_str());

  if(!file) {
    std::cout << fn << " : ShaderSource : open error.\n";
    return false;
  }

  std::string src=std::string((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

  //
  std::string fnPath;

  {
    size_t pos = fn.find_last_of("\\/");

    if(std::string::npos != pos) {
      fnPath=fn.substr(0,pos+1);
    }
  }

  //handle includes
  //todo : test for errors
  {
    unsigned int pos=0;
    unsigned int lastBeginningPos=0;
    bool notStart=false;

    while(pos < src.size()) {
      if(src[pos]==' ' || src[pos]=='\t' || src[pos]=='\r') {
        pos++;
      } else if(src[pos]=='\n') {
        notStart=false;
        pos++;
      } else if(src[pos]=='/' && pos+1 < src.size() && src[pos+1]=='/') {
        pos+=2;

        while(pos < src.size() && src[pos]!='\n') {
          pos++;
        }

        notStart=false;
      } else if(src[pos]=='/' && pos+1 < src.size()-1 && src[pos+1]=='*') {
        pos+=2;

        while(pos+1< src.size() && src[pos]!='*' && src[pos+1]!='/') {
          pos++;
        }
      } else if(!notStart && src[pos]=='#') { //
        unsigned int hashStart=pos;
        pos++;
        notStart=true;

        //whitespace
        while(src[pos]==' ' || src[pos]=='\t') {
          pos++;
        }

        //include
        if(pos+7<src.size() && src[pos]=='i' && src[pos+1]=='n' && src[pos+2]=='c' && 
          src[pos+3]=='l' && src[pos+4]=='u' && src[pos+5]=='d' && src[pos+6]=='e') {

          pos+=7;

          //whitespace
          while(src[pos]==' ' || src[pos]=='\t') {
            pos++;
          }

          //quote
          if(src[pos]=='"') {
            pos++;
            unsigned int startPos=pos;

            while(src[pos++]!='"'); //pos=after quote

            //
            unsigned int endPos=pos;
            std::string includeFn=src.substr(startPos,endPos-startPos-1);

            //
            if(hashStart>0) {              
              std::string src2=src.substr(lastBeginningPos,hashStart-lastBeginningPos);
              source->srcs.push_back(std::make_pair(src2,false));
            }

            source->srcs.push_back(std::make_pair(fnPath+includeFn,true));
            lastBeginningPos=endPos; //including newline after include directive

          }

          //whitespace
          while(src[pos]==' ' || src[pos]=='\t') {
            pos++;
          }

          //newline or eof
          if(pos==src.size() || src[pos]=='\n') {
            if(pos<src.size()) {
              pos++;
            }
          } else {
            //leave it to be picked up by the compiler as a syntax error
            pos++;
          }
        } else {
          //leave it to be picked up by the compiler as a syntax error
          pos++;
        }
      } else {
        notStart=true;
        pos++;
      }
    }

    //
    if(lastBeginningPos!=src.size()) {
      std::string src2=src.substr(lastBeginningPos,src.size()-lastBeginningPos)+"\n";
      source->srcs.push_back(std::make_pair(src2,false));
    }
  }

  //

  std::cout << fn << " : ShaderSource : loaded.\n";
  source->ok=true;
  return true;
}

bool ShaderManager::loadShader(Shader *shader,GLenum type,const std::string &fn) {

  shader->ok=false;
  //shader->programDeps.clear();

  //
  std::string txt;

  std::list<std::pair<std::string,bool>> srcStack;
  srcStack.push_back(std::make_pair(fn,true));

  while(srcStack.size()) {
    std::pair<std::string,bool> cur=srcStack.front();
    srcStack.pop_front();

    if(cur.second) {
      Source *source=getSource(cur.first);
      shader->sourceDeps.insert(cur.first);

      if(type==GL_VERTEX_SHADER) {
        source->vertexShaderDeps.insert(fn);
      } else if(type==GL_GEOMETRY_SHADER) {
        source->geometryShaderDeps.insert(fn);
      } else if(type==GL_FRAGMENT_SHADER) {
        source->fragmentShaderDeps.insert(fn);
      }

      if(!source->ok) {
        //std::cout << fn << " : Shader include source open error.\n";
        return false;
      }

      srcStack.insert(srcStack.begin(),source->srcs.begin(),source->srcs.end());
    } else {
      txt+=cur.first;
    }
  }

  //
  std::string head;


  head+="#version 130\n";
  head+="#extension GL_ARB_shading_language_420pack : enable\n";
  head+="#extension GL_ARB_shading_language_packing : enable\n";
  head+="#extension GL_ARB_uniform_buffer_object : enable\n";

  if(type==GL_VERTEX_SHADER) {
    head+="#extension GL_ARB_explicit_attrib_location : enable\n";
  }

  if(type==GL_GEOMETRY_SHADER) {
    head+="#extension GL_ARB_geometry_shader4 : enable\n";
  }

  head+="\n#line 0\n";

  //
  const char *sources[2]={ head.c_str(),txt.c_str() };

  glShaderSource(shader->obj,2,sources,0);
  glCompileShader(shader->obj);

  GLint status;
  glGetShaderiv(shader->obj,GL_COMPILE_STATUS,&status);

  if(!status) {
    char log[1024];
    glGetShaderInfoLog(shader->obj,1024,0,log);
    //std::cout << "`" << txt << "'\n";
    std::cout << fn << " : Shader : compile error.\n";
    std::cout << log << std::endl;

    return false;
  }

  shader->ok=true;


  std::cout << fn << " : ";

  if(type==GL_VERTEX_SHADER) {
    std::cout << "Vertex";
  } else if(type==GL_GEOMETRY_SHADER) {
    std::cout << "Geometry";
  } else if(type==GL_FRAGMENT_SHADER) {
    std::cout << "Fragment";
  }

  std::cout << "Shader : compiled.\n";
  return true;
}

bool ShaderManager::loadProgram(Program *program,const std::string &vsFn,const std::string &gsFn,const std::string &fsFn) {
  std::string key=vsFn+ " & " + gsFn + " & " + fsFn;

  //
  program->ok=false;
  program->vertexShaderDeps.clear();
  program->geometryShaderDeps.clear();
  program->fragmentShaderDeps.clear();
  bool shaderError=false;

  if(!vsFn.empty()) {
    program->vertexShaderDeps.insert(vsFn);
    Shader *shader=getVertexShader(vsFn);
    shader->programDeps.insert(key);

    if(!shader->ok) {
      shaderError=true;
    }

    glAttachShader(program->obj,shader->obj);
  }


  if(!gsFn.empty()) {
    program->geometryShaderDeps.insert(gsFn);
    Shader *shader=getGeometryShader(gsFn);
    shader->programDeps.insert(key);

    if(!shader->ok) {
      shaderError=true;
    }

    glAttachShader(program->obj,shader->obj);
  }


  if(!fsFn.empty()) {
    program->fragmentShaderDeps.insert(fsFn);
    Shader *shader=getFragmentShader(fsFn);
    shader->programDeps.insert(key);

    if(!shader->ok) {
      shaderError=true;
    }

    glAttachShader(program->obj,shader->obj);
  }

  //
  if(shaderError) {
    return false;
  }

  //
  if(!linkProgram(program,key)) {
    return false;
  }

  //
  program->ok=true;
  return true;
}

bool ShaderManager::linkProgram(Program *program,const std::string &key) {
  glLinkProgram(program->obj);

  GLint status;
  glGetProgramiv(program->obj,GL_LINK_STATUS,&status);

  if(!status) {
    char log[1024];
    glGetProgramInfoLog(program->obj,1024,0,log);
    glDeleteProgram(program->obj);
    std::cout << key << ": Program : link error, " << log << std::endl;
    return false;
  }

  return true;
}
