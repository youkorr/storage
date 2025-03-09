#pragma once

#include "esphome/core/component.h"
#include <vector>
#include <string>

namespace esphome {
namespace storage {

class StorageFile {
 public:
  void set_path(const std::string &path) { this->path_ = path; }
  void set_id(const std::string &id) { this->id_ = id; }
  std::string get_path() const { return path_; }
  std::string get_id() const { return id_; }

 private:
  std::string path_;
  std::string id_;
};

class StorageComponent : public Component {
 public:
  void set_platform(const std::string &platform) { platform_ = platform; }
  void add_file(const StorageFile &file) {
    files_.push_back(file);
  }
  std::string get_file_path(const std::string &id) const;
  void setup() override;

 protected:
  void setup_sd_card();
  void setup_flash();
  void setup_inline();

 private:
  std::string platform_;
  std::vector<StorageFile> files_;
};

}  // namespace storage
}  // namespace esphome
















