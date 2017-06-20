#include "Vao.h"

Vao::Vao() {

}
Vao::iterator Vao::begin() {
  return attribs.begin();
}
Vao::iterator Vao::end() {
  return attribs.end();
}
void Vao::addAttrib(int index, std::string name) {
  attribs.push_back(std::make_pair(index,name));
}
