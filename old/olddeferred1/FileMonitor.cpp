#include "FileMonitor.h"


FileMonitor::FileMonitor() {
}


FileMonitor::~FileMonitor() {
}
void FileMonitor::addWatch(const std::string &path) {
  fileWatcher.addWatch(path, this);
}
  const std::list<std::string> &FileMonitor::getAdded() {
    return added;
}
  const std::list<std::string> &FileMonitor::getDeleted() {
    return deleted;
}
  const std::list<std::string> &FileMonitor::getModified() {
    return modified;
}
  
  void FileMonitor::handleFileAction(FW::WatchID watchid,const FW::String& dir,const FW::String& filename,FW::Action action) {
    if(action == FW::Action::Add) {
      added.push_back(dir + "/" +filename);
    } else if(action == FW::Action::Modified) {
      modified.push_back(dir + "/" +filename);
    } else if(action == FW::Action::Delete) {
      deleted.push_back(dir + "/" +filename);
    }
  }
  
  void FileMonitor::run() {
    added.clear();
    modified.clear();
    deleted.clear();
    fileWatcher.update();
  }