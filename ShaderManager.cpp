#include "ShaderManager.h"
#include <iostream>
#include <fstream>

ShaderManager::ShaderManager() {
}

ShaderManager::~ShaderManager() {
}

GLuint ShaderManager::get(const std::string &vsFn,const std::string &gsFn,const std::string &fsFn,
  const std::string &vsDefs,const std::string &gsDefs,const std::string &fsDefs) {
  Program *program=getProgram(vsFn,gsFn,fsFn,vsDefs,gsDefs,fsDefs);
  return program->obj;
}

bool ShaderManager::reloadSource(const std::string &fn)  {
  auto it=sources.find(fn);

  if(it==sources.end()) {
    return false;
  }

  //
  Source *source=it->second;
  reloadSource(source,fn,true);
  return true;
}
bool ShaderManager::reloadSource(Source *source,const std::string &fn,bool force) {

  
  if(force) {
    source->modified.reset();
  }
  if(!loadSource(source,fn)) {
    return false;
  }

  for(auto shaderKey : source->shaderDeps) {
    auto shaderIt=shaders.find(shaderKey);

    if(shaderIt==shaders.end()) {
      continue;
    }

    Shader *shader=shaderIt->second;

    if(!loadShader(shader,shaderKey)) {
      continue;
    }

    for(auto programKey : shader->programDeps) {
      Program *program=programs.find(programKey)->second;
      linkProgram(program,programKey);
      //std::cerr << programKey << " relinked.\n";
    }
  }

  return true;
}

bool ShaderManager::removeProgram(const std::string &vsFn,const std::string &gsFn,const std::string &fsFn,
  const std::string &vsDefs,const std::string &gsDefs,const std::string &fsDefs) {


  std::string key=vsFn+ " & " + gsFn + " & " + fsFn + " & " + vsDefs+ " & " + gsDefs + " & " + fsDefs;

  auto it=programs.find(key);

  if(it==programs.end()) {
    return false;
  }

  //
  Program *program=it->second;



  for(auto shaderDep : program->shaderDeps) {
    auto shaderIt=shaders.find(shaderDep);

    if(shaderIt!=shaders.end()) {
      Shader *shader=shaderIt->second;
      shader->programDeps.erase(key);

      if(shader->programDeps.empty()) {
        Source *source=getSource(shader->sourceDep);
        source->shaderDeps.erase(shaderDep);

        if(source->shaderDeps.empty()) {
          sources.erase(shader->sourceDep);
          delete source;
        }

        //
        shaders.erase(shaderDep);
        glDetachShader(program->obj,shader->obj);
        glDeleteShader(shader->obj);
        delete shader;
      }
    }
  }

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

  for(auto i=shaders.begin();i!=shaders.end();i++) {
    glDeleteShader(i->second->obj);
    delete i->second;
  }

  for(auto i=sources.begin();i!=sources.end();i++) {
    delete i->second;
  }

  programs.clear();
  shaders.clear();
  sources.clear();
}

void ShaderManager::refresh() {
  for(auto sourceIt : sources) {
    const std::string &fn= sourceIt.first;
    Source *source=sourceIt.second;


    reloadSource(source, fn,false);
  }
}

ShaderManager::Source *ShaderManager::getSource(const std::string &fn) {
  auto it=sources.find(fn);

  if(it!=sources.end()) {
    return it->second;
  }

  //
  //std::cerr << "New Source " << fn << std::endl;

  //
  Source *source=new Source();
  source->ok=false;
  sources.insert(std::make_pair(fn,source));

  loadSource(source,fn);

  return source;
}

std::map<std::string,ShaderManager::Shader*>::iterator ShaderManager::getShader(GLenum type,const std::string &fn,const std::string &defs) {
  std::string defsKey,defsSrc;

  {
    std::stringstream ss(defs);
    std::string def;

    while(std::getline(ss,def,';')) {
      defsKey+=def+";";
      defsSrc+="#define " + def + "\n";
    }
  }

  std::string typeKey;

  if(type==GL_VERTEX_SHADER) {
    typeKey="v";
  } else if(type==GL_GEOMETRY_SHADER) {
    typeKey="g";
  } else if(type==GL_FRAGMENT_SHADER) {
    typeKey="f";
  }

  std::string key = typeKey + " & " + fn + " & " + defsKey;

  //
  auto it=shaders.find(key);

  if(it!=shaders.end()) {
    return it;
  }

  //
  Shader *shader=new Shader();
  it=shaders.insert(std::make_pair(key,shader)).first;
  shader->obj=glCreateShader(type);
  shader->defsKey=defsKey;
  shader->defsSrc=defsSrc;
  shader->type=type;
  shader->sourceDep=fn;

  loadShader(shader,key);

  return it;
}

ShaderManager::Program *ShaderManager::getProgram(const std::string &vsFn,const std::string &gsFn,const std::string &fsFn,
  const std::string &vsDefs,const std::string &gsDefs,const std::string &fsDefs) {
  std::string key=vsFn+ " & " + gsFn + " & " + fsFn + " & " + vsDefs+ " & " + gsDefs + " & " + fsDefs;

  auto it=programs.find(key);

  if(it!=programs.end()) {
    return it->second;
  }

  Program *program = new Program();
  programs.insert(std::make_pair(key,program));
  program->obj=glCreateProgram();

  //
  loadProgram(program,vsFn,gsFn,fsFn, vsDefs,gsDefs,fsDefs);
  return program;
}
bool ShaderManager::loadSource(Source *source,const std::string &fn) {

  if(!source->modified.update(fn)) {
    return false;
  }

  //
  source->ok=false;

  //
  std::ifstream file(fn.c_str());

  if(!file) {
    std::cerr << fn << " : ShaderSource : open error / not found.\n";
    return false;
  }

  source->src=std::string((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());

  //
  std::cerr << fn << " : ShaderSource : loaded.\n";
  source->ok=true;
  return true;
}

bool ShaderManager::loadShader(Shader *shader, const std::string &key) {
  std::string &fn=shader->sourceDep;
  GLenum type=shader->type;
  shader->ok=false;
  Source *source=getSource(fn);
  source->shaderDeps.insert(key);
  shader->src="";
  //
  if(!shader->defsKey.empty()){
    int lineCount=0;
    int beforeLineCount=0;
    unsigned int pos=0;
    unsigned int lastPos=0;
    std::string &src=source->src;

    while(pos < src.size()) {
      if(src[pos]==' ' || src[pos]=='\t' || src[pos]=='\r') { //skip white space
        pos++;
      } else if(src[pos]=='\n') { //skip newlines
        lineCount++;
        pos++;
      } else if(src[pos]=='/' && pos+1 < src.size() && src[pos+1]=='/') { //skip single line comments
        pos+=2;

        while(pos < src.size() && src[pos]!='\n') {
          pos++;
        }

        lineCount++;
        pos++;
      } else if(src[pos]=='/' && pos+1 < src.size()-1 && src[pos+1]=='*') { //skip double lines comments
        pos+=2;

        while(pos+1< src.size() && src[pos]!='*' && src[pos+1]!='/') {
          if(src[pos]=='\n') {
            lineCount++;
          }

          pos++;
        }
      } else if(src[pos]=='#') { //
        unsigned int hashStart=pos;
        pos++;

        //whitespace
        while(src[pos]==' ' || src[pos]=='\t') {
          pos++;
        }

        if(pos+7<src.size() && src[pos]=='v' && src[pos+1]=='e' && src[pos+2]=='r' &&
          src[pos+3]=='s' && src[pos+4]=='i' && src[pos+5]=='o' && src[pos+6]=='n') { //version

          pos+=7;
          while(src[pos++]!='\n'); //pos=after version line
          lineCount++;
          beforeLineCount+=lineCount;
          lineCount=0;
          lastPos=pos;
        } else if(pos+9<src.size() && src[pos]=='e' && src[pos+1]=='x' && src[pos+2]=='t' &&
          src[pos+3]=='e' && src[pos+4]=='n' && src[pos+5]=='s' && src[pos+6]=='i' &&
          src[pos+7]=='o' && src[pos+8]=='n') { //extension

          pos+=9;
          while(src[pos++]!='\n'); //pos=after version line
          lineCount++;
          beforeLineCount+=lineCount;
          lineCount=0;
          lastPos=pos;
        } else { //an error?
          //continue anyway, not my problem
          pos++;
        }
      } else { //version/extension directives not at head of file
        //continue anyway, not my problem
        pos++;
      }
    }


    std::stringstream ss;
    ss <<"#line " <<(beforeLineCount+1) <<"\n";
    std::string line=ss.str();


    //const GLchar *sources[4]={ source->src.c_str(),shader->defsSrc.c_str(),line.c_str(),&source->src.c_str()[lastPos] };

   // const GLint sourceSizes[4]={ lastPos,shader->defsSrc.size(),line.size(),source->src.size()-lastPos };

   // glShaderSource(shader->obj,4,sources,sourceSizes);

   shader->src=
     src.substr(0,lastPos) +
     shader->defsSrc +
     line+
     src.substr(lastPos,source->src.size()-lastPos);

   const GLchar *sources[1]={ shader->src.c_str() };


   glShaderSource(shader->obj,1,sources,0);
    if(false){
    char srcc[15000];
    glGetShaderSource(shader->obj,15000,0,srcc);
    std::ofstream outfile;
    outfile.open("test.txt",std::ios_base::app);
    outfile << "----\n" <<srcc << "\n";
    // system("pause");
  }
  } else {
    const GLchar *sources[]={ source->src.c_str() };
    glShaderSource(shader->obj,1,sources,0);
  }
  //

  //const GLchar *sources[3]={ before.c_str(),middle.c_str(),after.c_str() };
   //const GLint sourceSizes[3]={ before.size(),middle.size(),after.size() };
  //std::cerr << ":" << before << ":" << middle << ":" << after << ":";
  //system("pause");
  glCompileShader(shader->obj);

  GLint status;
  glGetShaderiv(shader->obj,GL_COMPILE_STATUS,&status);

  if(!status) {
    char log[1024];
    glGetShaderInfoLog(shader->obj,1024,0,log);
    //std::cerr << "`" << txt << "'\n";
    std::cerr << key << " : Shader : compile error.\n";
    std::cerr << log << std::endl;

    return false;
  }

  shader->ok=true;


  std::cerr << key << " : ";

  if(type==GL_VERTEX_SHADER) {
    std::cerr << "Vertex";
  } else if(type==GL_GEOMETRY_SHADER) {
    std::cerr << "Geometry";
  } else if(type==GL_FRAGMENT_SHADER) {
    std::cerr << "Fragment";
  }

  std::cerr << "Shader : compiled.\n";
  return true;
}

bool ShaderManager::loadProgram(Program *program,const std::string &vsFn,const std::string &gsFn,const std::string &fsFn,
  const std::string &vsDefs,const std::string &gsDefs,const std::string &fsDefs) {

  std::string key=vsFn+ " & " + gsFn + " & " + fsFn + " & " + vsDefs+ " & " + gsDefs + " & " + fsDefs;


  //
  program->ok=false;
  //program->shaderDeps.clear();
  bool shaderError=false;

  if(!vsFn.empty()) {
   auto shaderIt=getShader(GL_VERTEX_SHADER,vsFn,vsDefs);
   program->shaderDeps.insert(shaderIt->first);
   Shader *shader=shaderIt->second;
   shader->programDeps.insert(key);

    if(!shader->ok) {
      shaderError=true;
    }

    glAttachShader(program->obj,shader->obj);
  }

  if(!gsFn.empty()) {
    auto shaderIt=getShader(GL_GEOMETRY_SHADER,gsFn,gsDefs);
    program->shaderDeps.insert(shaderIt->first);
    Shader *shader=shaderIt->second;
    shader->programDeps.insert(key);

    if(!shader->ok) {
      shaderError=true;
    }

    glAttachShader(program->obj,shader->obj);
  }

  if(!fsFn.empty()) {
    auto shaderIt=getShader(GL_FRAGMENT_SHADER,fsFn,fsDefs);
    program->shaderDeps.insert(shaderIt->first);
    Shader *shader=shaderIt->second;
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
    std::cerr << key << ": Program : link error, " << log << std::endl;
    return false;
  }

  return true;
}

std::string ShaderManager::getShaderSource(GLenum type,const std::string &fn,const std::string &defs) {
  auto it=getShader(type,fn,defs);

  if(it==shaders.end()) {
    return "";
  }

  return it->second->src;
}