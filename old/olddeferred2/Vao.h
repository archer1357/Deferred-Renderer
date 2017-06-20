

#include <list>
#include <string>

class Vao {
public:
  typedef std::list<std::pair<int,std::string> >::iterator iterator;
private:
  std::list<std::pair<int,std::string> > attribs;
public:
  Vao();
  iterator begin();
  iterator end();
  void addAttrib(int index, std::string name);
};
