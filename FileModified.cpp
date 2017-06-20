#include "FileModified.h"

FileModified::FileModified()
{
  reset();
}


FileModified::~FileModified()
{
}

bool FileModified::update(const std::string &fn) {
#if defined WIN32

  HANDLE fh;
  FILETIME ft;

#ifdef UNICODE
  wchar_t fn2[512];
  mbstowcs(fn2,fn.c_str(),512);
#else
  const char *fn2=fn.c_str();
#endif

  //
  fh=CreateFile(fn2,GENERIC_READ,
    FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE,
    NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);

  if(fh==INVALID_HANDLE_VALUE) {
    return false;
  }

  //
  if(0==GetFileTime(fh,NULL,NULL,&ft)) {
    CloseHandle(fh);
    return false;
  }

  //
  CloseHandle(fh);

  //
  if(0==CompareFileTime(&ft,&this->modified)) {
    return false;
  }

  //
  memcpy(&this->modified,&ft,sizeof(FILETIME));

#elif defined LINUX
#else
#endif

return true;
}

void FileModified::reset() {
#if defined WIN32
  ZeroMemory(&this->modified,sizeof(FILETIME));
#elif defined LINUX
#else
#endif

}