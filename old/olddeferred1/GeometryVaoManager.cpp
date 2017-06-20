#include "GeometryVaoManager.h"


GeometryVaoManager::GeometryVaoManager() {
  modeMap["points"]=GL_POINTS;
  modeMap["line_strip"]=GL_LINE_STRIP;
  modeMap["line_loop"]=GL_LINE_LOOP;
  modeMap["lines"]=GL_LINES;
  modeMap["line_strip_adjacency"]=GL_LINE_STRIP_ADJACENCY;
  modeMap["lines_adjacency"]=GL_LINES_ADJACENCY;
  modeMap["triangle_strip"]=GL_TRIANGLE_STRIP;
  modeMap["triangle_fan"]=GL_TRIANGLE_FAN;
  modeMap["triangles"]=GL_TRIANGLES;
  modeMap["triangle_strip_adjacency"]=GL_TRIANGLE_STRIP_ADJACENCY;
  modeMap["triangles_adjacency"]=GL_TRIANGLES_ADJACENCY;
  modeMap["patches"]=GL_POINTS;
  modeMap["points"]=GL_PATCHES;

  typeMap["float"]=GL_FLOAT;
  typeMap["uint"]=GL_UNSIGNED_INT;
  typeMap["ushort"]=GL_UNSIGNED_SHORT;

  positionsVao=new Vao();
}

GeometryVaoManager::~GeometryVaoManager() {
  delete positionsVao;

  for(std::map<std::string,Geometry*>::iterator i=geoms.begin();i!=geoms.end();i++) {
    delete i->second;
  }

  for(std::map<std::string,Vao*>::iterator i=vaos.begin();i!=vaos.end();i++) {
    delete i->second;
  }

  for(std::map<std::string,GeometryVao*>::iterator i=geomVaos.begin();i!=geomVaos.end();i++) {
    delete i->second;
  }
}


bool GeometryVaoManager::convStrMode(const std::string &s, GLenum &out) {
  std::map<std::string,GLenum>::iterator it=modeMap.find(s);

  if(it==modeMap.end()) {
    return false;
  }

  out=it->second;
  return true;
}

bool  GeometryVaoManager::convStrType(const std::string &s, GLenum &out) {
  std::map<std::string,GLenum>::iterator it=typeMap.find(s);

  if(it==typeMap.end()) {
    return false;
  }

  out=it->second;
  return true;
}

Geometry *GeometryVaoManager::getGeometry(const std::string &fn) {
  std::map<std::string,Geometry*>::iterator it=geoms.find(fn);


  if(it != geoms.end()) {
    return it->second;
  }

  //
  std::string fn2=fn;
  jsoncons::json doc;
  Geometry *geometry = new Geometry();
  try {
    doc = jsoncons::json::parse_file(fn2);

    //
    if(doc.has_member("vertices")) {
      jsoncons::json obj=doc["vertices"];

      if(obj.is_object()) {
        for(auto i=obj.begin_members();i!=obj.end_members();++i) {
          jsoncons::json vertObj=i->value();
          //type
          GLenum type;

          if(!convStrType(vertObj["type"].as_string(), type)) {
            std::cout << "Invalid type on " << fn <<"\n";
          }

          //size
          int size=vertObj["size"].as_int();

          //data
          jsoncons::json dataObj=vertObj["data"];

          if(dataObj.is_array()) {
            if(type==GL_FLOAT) {
              std::vector<float> data;//=dataObj.as<std::vector<float>>();
              for(auto j=dataObj.begin_elements();j!=dataObj.end_elements();++j) {
                data.push_back((float)j->as_double());
              }

              int dataSize=data.size()*sizeof(float);
              geometry->addVertices(i->name(),type,size,dataSize,&data[0]);
            }
          } else if(dataObj.is_string()) {
            std::string b=base64_decode(dataObj.as_string());
            geometry->addVertices(i->name(),type,size,b.size(),b.c_str());
          }
        }
      }
    }

    if(doc.has_member("indices")) {

      jsoncons::json obj=doc["indices"];

      if(obj.is_object()) {
        //type
        GLenum type;

        if(!convStrType(obj["type"].as_string(), type)) {
            std::cout << "Invalid type on " << fn <<"\n";
        }

        //data
        jsoncons::json dataObj=obj["data"];

        if(dataObj.is_array()) {
          if(type==GL_UNSIGNED_SHORT) {
            std::vector<unsigned short> data;//=dataObj.as<std::vector<float>>();
            for(auto j=dataObj.begin_elements();j!=dataObj.end_elements();++j) {
              data.push_back(j->as_int());
            }

            int dataSize=data.size()*sizeof(short);
            geometry->setIndices(type,dataSize,&data[0]);
          } else if(type==GL_UNSIGNED_INT) {
            std::vector<unsigned int> data;//=dataObj.as<std::vector<float>>();
            for(auto j=dataObj.begin_elements();j!=dataObj.end_elements();++j) {
              data.push_back(j->as_int());
            }

            int dataSize=data.size()*sizeof(int);
            geometry->setIndices(type,dataSize,&data[0]);
          }
        } else if(dataObj.is_string()) {
          std::string b=base64_decode(dataObj.as_string());
          geometry->setIndices(type,b.size(),b.c_str());
        }
      }
    }

    if(doc.has_member("draws")) {
      jsoncons::json obj=doc["draws"];

      if(obj.is_object()) {
        for(auto i=obj.begin_members();i!=obj.end_members();++i) {
          jsoncons::json drawObj=i->value();
          //mode
          GLenum mode=0;

          if(!convStrMode(drawObj["mode"].as_string(),mode)) {
            std::cout << "Invalid draw mode on " << fn <<"\n";
          }

          //first
          int first=drawObj["first"].as_int();

          //count
          int count=drawObj["count"].as_int();

          //
          if(GeometryIndices * geomIndices=geometry->getIndices()) {
            geometry->addDrawElements(i->name(),mode,first,count,geomIndices->getType());
          } else {
            geometry->addDrawArrays(i->name(),mode,first,count);
          }
        }
      }
    }


    //
  } catch (const std::exception& e) {
    std::cout << fn << " " << e.what() << std::endl;
    return 0;
  }
  geoms.insert(std::make_pair(fn,geometry));
  return geometry;
}


Vao *GeometryVaoManager::getVao(const std::string &fn) {
  std::map<std::string,Vao*>::iterator it=vaos.find(fn);

  if(it!=vaos.end()) {
    return it->second;
  }

  //


  std::string fn2=fn;
  std::string geomFn,vaoFn;
  jsoncons::json doc;
  Vao *vao=new Vao;
  try {
    doc = jsoncons::json::parse_file(fn2);

    //

    for(auto i=doc.begin_members();i!=doc.end_members();++i) {
      if(i->value().is_numeric()) {
        vao->addAttrib(i->value().as_int(),i->name());
      }
    }

  } catch (const std::exception& e) {
    std::cout << fn << " " << e.what() << std::endl;
    return 0;
  }
  vaos.insert(std::make_pair(fn,vao));
  return vao;
}


GeometryVao *GeometryVaoManager::get(const std::string &geomFn,const std::string &vaoFn) {
  if(geomFn.empty() || vaoFn.empty()) {
    return 0;
  }

  std::string key=geomFn+" & "+vaoFn;

  auto it=geomVaos.find(key);

  if(it !=geomVaos.end()) {
    return it->second;
  }

  Geometry *geometry=getGeometry(geomFn);
  Vao *vao=getVao(vaoFn);

  if(!geometry || !vao) {
    geomVaos.insert(std::make_pair(key,(GeometryVao*)0));
    return 0;
  }

  GeometryVao *geomVao=new GeometryVao(geometry,vao);
  geomVaos.insert(std::make_pair(key,geomVao));

  return geomVao;
}


void GeometryVaoManager::onFileAdded(const std::string &fn) {
}
void GeometryVaoManager::onFileModified(const std::string &fn) {
  //todo

}
void GeometryVaoManager::onFileDeleted(const std::string &fn) {
}
void  GeometryVaoManager::clear() {
  geomVaos.clear();
}


ShadowVolumeCpuGeometry *GeometryVaoManager::getShadowVolumeCpuGeometry(const std::string &geomFn) {













  if(geomFn.empty()  ) {
    return 0;
  }

  auto it=shadowVolumeCpuGeometries.find(geomFn);

  if(it!=shadowVolumeCpuGeometries.end()) {
    return it->second;
  }

  ShadowVolumeCpuGeometry *geom=new ShadowVolumeCpuGeometry();
  shadowVolumeCpuGeometries.insert(std::make_pair(geomFn,geom));

  std::string fn2=geomFn;

  try {
    jsoncons::json doc;
    doc = jsoncons::json::parse_file(fn2);

    if(doc.has_member("vertices")) {
      jsoncons::json obj=doc["vertices"];

      if(obj.is_object() && obj.has_member("positions")) {
        jsoncons::json vertObj=obj["positions"];
        //type
        GLenum type;

        if(!convStrType(vertObj["type"].as_string(), type)) {
          std::cout << "Invalid type on " << geomFn <<"\n";
        }

        //size
        int size=vertObj["size"].as_int();

        //data
        jsoncons::json dataObj=vertObj["data"];

        if(dataObj.is_array()) {
          if(type==GL_FLOAT) {
            std::vector<float> data;//=dataObj.as<std::vector<float>>();
            for(auto j=dataObj.begin_elements();j!=dataObj.end_elements();++j) {
              data.push_back((float)j->as_double());
            }

            geom->setVertices(&data[0],(int)data.size());
          }
        } else if(dataObj.is_string()) {
          std::string b=base64_decode(dataObj.as_string());
          geom->setVertices((const float*)b.c_str(),(int)b.size()/sizeof(float));
        }
      }
    }

    if(doc.has_member("indices")) {
      jsoncons::json obj=doc["indices"];

      if(obj.is_object()) {
        //type
        GLenum type;

        if(!convStrType(obj["type"].as_string(), type)) {
          std::cout << "Invalid type on " << geomFn <<"\n";
        }

        //data
        jsoncons::json dataObj=obj["data"];

        if(dataObj.is_array()) {
          if(type==GL_UNSIGNED_SHORT) {
            std::vector<unsigned short> data;

            for(auto j=dataObj.begin_elements();j!=dataObj.end_elements();++j) {
              data.push_back(j->as_int());
            }

            geom->setIndices(&data[0],(int)data.size());
          } else if(type==GL_UNSIGNED_INT) {
            std::vector<unsigned int> data;

            for(auto j=dataObj.begin_elements();j!=dataObj.end_elements();++j) {
              data.push_back(j->as_int());
            }

            geom->setIndices(&data[0],(int)data.size());
          }
        } else if(dataObj.is_string()) {
          std::string b=base64_decode(dataObj.as_string());

          if(type==GL_UNSIGNED_SHORT) {
            geom->setIndices((const unsigned short*)b.c_str(),(int)b.size()/sizeof(short));
          } else if(type==GL_UNSIGNED_INT) {
            geom->setIndices((const unsigned int*)b.c_str(),(int)b.size()/sizeof(int));
          }

          // geometry->setIndices(type,b.size(),b.c_str());
        }
      }
    }

    geom->calcNormals();

  } catch (const std::exception& e) {
    std::cout << geomFn << " " << e.what() << std::endl;
    return 0;
  }

  return geom;
}
