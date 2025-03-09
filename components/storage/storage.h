#pragma once

#include <string>
#include <vector>
#include "esphome/core/component.h"

namespace esphome {
namespace storage {

class StorageFile {
 public:
  StorageFile(const std::string &path, const std::string &id) : path_(path), id_(id) {}
  
  const std::string &get_path() const { return path_; }
  const std::string &get_id() const { return id_; }

 private:
  std::string path_;
  std::string id_;
};

class StorageComponent : public Component {
 public:
  void setup() override;
  std::string get_file_path(const std::string &file_id) const;
  void add_file(const std::string &path, const std::string &id) { files_.emplace_back(path, id); }

 private:
  void setup_sd_card();
  void setup_flash();
  void setup_inline();

  std::vector<StorageFile> files_;
  std::string platform_;
};

}  // namespace storage
}  // namespace esphome
















