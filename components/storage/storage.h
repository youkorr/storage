#pragma once
#include <string>
#include <vector>
#include "esphome/core/component.h"
#include "esphome/components/audio/audio.h"

// Forward declaration of SDMMCCard class
namespace esphome {
namespace sd_mmc_card {
class SDMMCCard;
}  // namespace sd_mmc_card
}  // namespace esphome

namespace esphome {
namespace storage {

class StorageFile : public audio::AudioFile, public Component {
 public:
  StorageFile() : path_(""), id_(""), platform_("") {}
  StorageFile(const std::string &path, const std::string &id) : path_(path), id_(id), platform_("") {}
  
  void setup() override {}
  
  void set_component_source(const std::string &source) { component_source_ = source; }
  const std::string &get_component_source() const { return component_source_; }
  
  const std::string &get_path() const { return path_; }
  const std::string &get_id() const { return id_; }
  const std::string &get_platform() const { return platform_; }
  void set_path(const std::string &path) { path_ = path; }
  void set_id(const std::string &id) { id_ = id; }
  void set_platform(const std::string &platform) { platform_ = platform; }
  
  std::string get_filename() const { return path_; }
  bool is_valid() const { return !path_.empty(); }
  
 private:
  std::string path_;
  std::string id_;
  std::string platform_;
  std::string component_source_;
};

class StorageComponent : public Component {
 public:
  void setup() override;
  std::string get_file_path(const std::string &file_id) const;
  void add_file(StorageFile *file) { files_.push_back(file); }
  
  void set_platform(const std::string &platform) { platform_ = platform; }
  void set_sd_mmc(sd_mmc_card::SDMMCCard *sd_card) { sd_card_ = sd_card; }
  
 private:
  void setup_sd_card();
  void setup_flash();
  void setup_inline();
  
  std::vector<StorageFile*> files_;
  std::string platform_;
  sd_mmc_card::SDMMCCard *sd_card_{nullptr};
};

}  // namespace storage
}  // namespace esphome

















