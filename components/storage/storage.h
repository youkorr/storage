#pragma once

#include <string>
#include <vector>
#include "esphome/core/component.h"

namespace esphome {
namespace storage {

class StorageFile {
 public:
  StorageFile() : path_(""), id_("") {}  // Constructeur par défaut
  StorageFile(const std::string &path, const std::string &id) : path_(path), id_(id) {}

  const std::string &get_path() const { return path_; }
  const std::string &get_id() const { return id_; }

  void set_path(const std::string &path) { path_ = path; }
  void set_id(const std::string &id) { id_ = id; }

 private:
  std::string path_;
  std::string id_;
};

class StorageComponent : public Component {
 public:
  void setup() override;
  std::string get_file_path(const std::string &file_id) const;
  void add_file(const std::string &path, const std::string &id) { files_.emplace_back(path, id); }
  
  void set_platform(const std::string &platform) { platform_ = platform; }  // Ajout de set_platform()

 private:
  void setup_sd_card();
  void setup_flash();
  void setup_inline();

  std::vector<StorageFile> files_;
  std::string platform_;
};

}  // namespace storage
}  // namespace esphome

















