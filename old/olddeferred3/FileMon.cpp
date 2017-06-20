#include "FileMon.h"
#include <iostream>

FileMon::FileMon(double delay) : delay(delay) {
#ifdef LINUX
  notify = inotify_init();

  if(notify < 0) {
    std::cout << strerror(errno) << std::endl;
    return;
  }
#endif
#ifdef WIN32
  ghMutex = CreateMutex(NULL,FALSE,NULL);

  if(ghMutex == NULL) {
    std::cout << "FileMon : CreateMutex error: " << GetLastError() << std::endl;
  }
#endif
}

FileMon::~FileMon() {
#ifdef WIN32
  for(auto i : mWatches) {
    WatchStruct* pWatch=i.second;

    DWORD dwWaitResult = WaitForSingleObject(ghMutex,INFINITE);

    while(true) {
      if(dwWaitResult==WAIT_OBJECT_0) {
        pWatch->mStopNow = TRUE;

        if(!ReleaseMutex(ghMutex)) {
          std::cout << "FileMon : Release mutex err.\n";
        }

        break;
      } else if(dwWaitResult==WAIT_ABANDONED) {
        break;
      }
    }

    CancelIo(pWatch->mDirHandle);

    ReadDirectoryChangesW(
      pWatch->mDirHandle,pWatch->mBuffer,sizeof(pWatch->mBuffer),FALSE,
      pWatch->mNotifyFilter,NULL,&pWatch->mOverlapped,0);

    if(!HasOverlappedIoCompleted(&pWatch->mOverlapped)) {
      SleepEx(5,TRUE);
    }

    CloseHandle(pWatch->mOverlapped.hEvent);
    CloseHandle(pWatch->mDirHandle);
    HeapFree(GetProcessHeap(),0,pWatch);
  }

  CloseHandle(ghMutex);
#endif

#ifdef LINUX
  if(notify) {
    for(auto i : watchMap) {
      inotify_rm_watch(notify,i.first); //unnecessary because of close below?
    }
  }

  if(notify) {
    close(notify);
  }
#endif
}

void FileMon::update() {
#ifdef WIN32
  MsgWaitForMultipleObjectsEx(0,NULL,0,QS_ALLINPUT,MWMO_ALERTABLE);

  //
  DWORD dwWaitResult = WaitForSingleObject(ghMutex,INFINITE);

  while(true) {
    if(dwWaitResult==WAIT_OBJECT_0) {
      updateChanges();

      if(!ReleaseMutex(ghMutex)) {
        std::cout << "FileMon : Release mutex err.\n";
      }

      break;
    } else if(dwWaitResult==WAIT_ABANDONED) {
      break;
    }
  }
#endif

#ifdef LINUX

  if(!notify) {
    return;
  }

  const int bufSize=((sizeof(struct inotify_event)+FILENAME_MAX)*1024);

  fd_set mDescriptorSet;
  FD_ZERO(&mDescriptorSet);
  FD_SET(notify,&mDescriptorSet);

  struct timeval mTimeOut;
  mTimeOut.tv_sec = 0;
  mTimeOut.tv_usec = 0;

  int ret = select(notify + 1,&mDescriptorSet,0,0,&mTimeOut);

  if(ret < 0) {
    perror("select");
  } else if(FD_ISSET(notify,&mDescriptorSet)) {
    ssize_t len,i = 0;
    char action[81+FILENAME_MAX] ={ 0 };
    char buff[bufSize] ={ 0 };
    len = read(notify,buff,bufSize);

    while(i < len) {
      struct inotify_event *pevent = (struct inotify_event *)&buff[i];
      std::string n=watchMap[pevent->wd]+"/"+std::string(pevent->name);
      time_t t=time(0);

      if(IN_MOVED_TO & pevent->mask || IN_CREATE & pevent->mask) {
        changeTimes.insert(std::make_pair(n,t));
      } else if(IN_CLOSE_WRITE & pevent->mask) {
        changeTimes.insert(std::make_pair(n,t));
      } else if(IN_MOVED_FROM & pevent->mask || IN_DELETE & pevent->mask) {
      }

      i += sizeof(struct inotify_event) + pevent->len;
    }
  }

  updateChanges();
#endif
}

bool FileMon::monitor(const std::string &path) {
#ifdef WIN32
#ifdef UNICODE
  wchar_t path2[512];
  MultiByteToWideChar(CP_ACP,0,path.c_str(),-1,path2,512);
#else
  const char *path2=path.c_str();
#endif

  WatchStruct* pWatch;
  size_t ptrsize = sizeof(*pWatch);
  pWatch = static_cast<WatchStruct*>(HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,ptrsize));

  pWatch->mDirHandle = CreateFile(path2,FILE_LIST_DIRECTORY,
    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,NULL,
    OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,NULL);

  pWatch->fileMon=this;

  if(pWatch->mDirHandle != INVALID_HANDLE_VALUE) {
    pWatch->mOverlapped.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
    pWatch->mNotifyFilter = FILE_NOTIFY_CHANGE_CREATION|FILE_NOTIFY_CHANGE_LAST_WRITE|FILE_NOTIFY_CHANGE_FILE_NAME;

    if(ReadDirectoryChangesW(
      pWatch->mDirHandle,pWatch->mBuffer,sizeof(pWatch->mBuffer),FALSE,
      pWatch->mNotifyFilter,NULL,&pWatch->mOverlapped,WatchCallback) != 0) {
      pWatch->mDirName=path;
      mWatches.insert(std::make_pair(path,pWatch));
      return true;
    } else {
      CloseHandle(pWatch->mOverlapped.hEvent);
      CloseHandle(pWatch->mDirHandle);
    }
  }

  HeapFree(GetProcessHeap(),0,pWatch);
  return false;
#endif

#ifdef LINUX
  int watch = inotify_add_watch(notify,path.c_str(),IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE | IN_MOVED_FROM | IN_DELETE);

  if(watch < 0) {
    if(errno == ENOENT) {
      std::cout << "File monitor: File/dir not found error.\n";
      return false;
    } else {
      std::cout << "File monitor: " << strerror(errno) << std::endl;;
      return false;
    }
  } else {
    watchMap[watch]=path;
  }
  return true;
#endif
}

const std::list<std::string> &FileMon::getChanges() {
  return changes;
}

void FileMon::updateChanges() {
  changes.clear();

  time_t t=time(0);

  auto it=changeTimes.begin();

  while(it!=changeTimes.end()) {
    if(difftime(t,it->second) >= delay) {
      changes.push_back(it->first);
      it=changeTimes.erase(it);
    } else {
      it++;
    }
  }
}

#ifdef WIN32
void  FileMon::WatchCallback(DWORD dwErrorCode,DWORD dwNumberOfBytesTransfered,LPOVERLAPPED lpOverlapped) {
  TCHAR szFile[MAX_PATH];
  PFILE_NOTIFY_INFORMATION pNotify;
  WatchStruct* pWatch = (WatchStruct*)lpOverlapped;
  size_t offset = 0;

  if(dwNumberOfBytesTransfered == 0) {
    return;
  }

  if(dwErrorCode == ERROR_SUCCESS) {
    do {
      pNotify = (PFILE_NOTIFY_INFORMATION)&pWatch->mBuffer[offset];
      offset += pNotify->NextEntryOffset;

#ifdef UNICODE
      int fnLen=pNotify->FileNameLength/sizeof(WCHAR)+1;
      fnLen=(fnLen<MAX_PATH)?fnLen:MAX_PATH;
      lstrcpynW(szFile,pNotify->FileName,fnLen);
#else
      int count = WideCharToMultiByte(CP_ACP,0,pNotify->FileName,
        pNotify->FileNameLength / sizeof(WCHAR),
        szFile,MAX_PATH - 1,NULL,NULL);

      szFile[count] = TEXT('\0');
#endif

      char vOut[512];
      wcstombs_s(NULL,vOut,sizeof(vOut),szFile,512);

      std::string n=pWatch->mDirName + "/" + vOut;
      time_t t=time(0);

      DWORD dwWaitResult = WaitForSingleObject(pWatch->fileMon->ghMutex,INFINITE);

      while(true) {
        if(dwWaitResult==WAIT_OBJECT_0) {
          if(pNotify->Action==FILE_ACTION_RENAMED_NEW_NAME||pNotify->Action==FILE_ACTION_ADDED) {
            pWatch->fileMon->changeTimes.insert(std::make_pair(n,t));
          } else if(pNotify->Action==FILE_ACTION_RENAMED_OLD_NAME||pNotify->Action==FILE_ACTION_REMOVED) {
          } else if(pNotify->Action==FILE_ACTION_MODIFIED) {
            pWatch->fileMon->changeTimes.insert(std::make_pair(n,t));
          }

          if(!ReleaseMutex(pWatch->fileMon->ghMutex)) {
            std::cout << "FileMon : Release mutex err.\n";
          }

          break;
        } else if(dwWaitResult==WAIT_ABANDONED) {
          break;
        }
      }

    } while(pNotify->NextEntryOffset != 0);
  }

  DWORD dwWaitResult = WaitForSingleObject(pWatch->fileMon->ghMutex,INFINITE);

  while(true) {
    if(dwWaitResult==WAIT_OBJECT_0) {
      if(!pWatch->mStopNow) {
        ReadDirectoryChangesW(
          pWatch->mDirHandle,pWatch->mBuffer,sizeof(pWatch->mBuffer),FALSE,
          pWatch->mNotifyFilter,NULL,&pWatch->mOverlapped,WatchCallback);
      }

      if(!ReleaseMutex(pWatch->fileMon->ghMutex)) {
        std::cout << "FileMon : Release mutex err.\n";
      }

      break;
    } else if(dwWaitResult==WAIT_ABANDONED) {
      break;
    }
  }


}

#endif
