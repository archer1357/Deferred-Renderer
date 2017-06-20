#ifndef FILEMODIFIED
#define FILEMODIFIED

#include <string>

#ifdef WIN32
#include <cstdlib>
#include <windows.h>
#endif

class FileModified {
private:
#if defined WIN32
  FILETIME modified;
#elif defined LINUX
#endif
public:
  FileModified();
  ~FileModified();
  bool update(const std::string &fn);
  void reset();
};

#endif