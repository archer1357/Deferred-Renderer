#ifndef FILEMONITOR_H
#define FILEMONITOR_H

#include <string>
#include <list>
#include "FileWatcher.h"

class FileMonitor : public FW::FileWatchListener {
private:
  FW::FileWatcher fileWatcher;
  std::list<std::string> added,deleted,modified;
public:
  FileMonitor();
  ~FileMonitor();
  void addWatch(const std::string &path);
  const std::list<std::string> &getAdded();
  const std::list<std::string> &getDeleted();
  const std::list<std::string> &getModified();
  void handleFileAction(FW::WatchID watchid,const FW::String& dir,const FW::String& filename,FW::Action action);
  void run();
};

#endif