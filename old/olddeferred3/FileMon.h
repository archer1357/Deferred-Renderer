#ifndef FILEMON_HPP
#define FILEMON_HPP

#include <string>
#include <list>
#include <map>
#include <ctime>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")
#endif

#ifdef LINUX
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/inotify.h>
#include <unistd.h>
#endif

class FileMon {
public:
  FileMon(double delay=0.75);
  ~FileMon();
  void update();
  bool monitor(const std::string &path);
  const std::list<std::string> &getChanges();
private:
  void updateChanges();
#ifdef WIN32
  struct WatchStruct {
    OVERLAPPED mOverlapped;
    HANDLE mDirHandle;
    BYTE mBuffer[32 * 1024];
    LPARAM lParam;
    DWORD mNotifyFilter;
    bool mStopNow;
    std::string mDirName;
    FileMon *fileMon;
  };
  HANDLE ghMutex;
  std::map<std::string,WatchStruct*> mWatches;
  static void CALLBACK WatchCallback(DWORD dwErrorCode,DWORD dwNumberOfBytesTransfered,LPOVERLAPPED lpOverlapped);
#endif

#ifdef LINUX
  int notify;
  std::map<int,std::string> watchMap;
#endif
  double delay;
  std::map<std::string,time_t> changeTimes;
  std::list<std::string> changes;
};

#endif
