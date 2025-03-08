#pragma once

#include "esphome/core/component.h"
#include "esphome/core/helpers.h"  // Pour to_string()
#include <vector>
#include <string>

// Options de stockage inspirées de <button class="citation-flag" data-index="8">
#define STORAGE_MODE_READONLY  0x01
#define STORAGE_MODE_WRITEONLY 0x02
#define STORAGE_MODE_READWRITE 0x03
#define STORAGE_CREATE_IF_MISSING 0x04  // Inspiré de <button class="citation-flag" data-index="8">

namespace esphome {
namespace storage {

class StorageFile : public Component {
 public:
  void set_path(const std::string &path) { path_ = path; }
  std::string get_path() const { return path_; }

  // Méthodes de base inspirées de <button class="citation-flag" data-index="3">
  virtual bool read_block(uint8_t *buffer, size_t size, size_t offset) = 0;
  virtual bool write_block(const uint8_t *buffer, size_t size, size_t offset) = 0;
  
  // Méthode de vérification de l'espace <button class="citation-flag" data-index="3">
  virtual bool has_free_space(size_t required_bytes) const = 0;

 protected:
  std::string path_;
};

class StorageComponent : public Component {
 public:
  void set_platform(const std::string &platform) { platform_ = platform; }
  void add_file(StorageFile *file) { files_.push_back(file); }

  // Méthode d'initialisation multi-plateforme <button class="citation-flag" data-index="1"><button class="citation-flag" data-index="3">
  void setup() override {
    if (platform_ == "sd_card") {
      initialize_sd_card();
    } else if (platform_ == "flash") {
      initialize_flash();
    } else if (platform_ == "inline") {
      initialize_inline();
    }
  }

 private:
  std::string platform_;
  std::vector<StorageFile*> files_;

  // Méthodes d'initialisation spécifiques <button class="citation-flag" data-index="1">
  void initialize_sd_card();
  void initialize_flash();
  void initialize_inline();
};

}  // namespace storage
}  // namespace esphome











