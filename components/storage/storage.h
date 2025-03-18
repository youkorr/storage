#pragma once

#include <string>
#include <vector>
#include "esphome/core/component.h"

namespace esphome {
namespace storage {

class StorageFile : public Component {
 public:
  StorageFile() : path_(""), id_(""), platform_("") {}
  StorageFile(const std::string &path, const std::string &id) : path_(path), id_(id), platform_("") {}

  const std::string &get_path() const { return path_; }
  const std::string &get_id() const { return id_; }
  const std::string &get_platform() const { return platform_; }

  void set_path(const std::string &path) { path_ = path; }
  void set_id(const std::string &id) { id_ = id; }
  void set_platform(const std::string &platform) { platform_ = platform; }

  void setup() override {}
  void dump_config() override {}

 private:
  std::string path_;
  std::string id_;
  std::string platform_;
};

class StorageComponent : public Component {
 public:
  void setup() override;
  std::string get_file_path(const std::string &file_id) const;
  void add_file(StorageFile *file) { files_.push_back(file); }
  
  void set_platform(const std::string &platform) { platform_ = platform; }

 private:
  void setup_sd_card();
  void setup_flash();
  void setup_inline();

  std::vector<StorageFile*> files_;
  std::string platform_;
};

}  // namespace storage
}  // namespace esphome

















