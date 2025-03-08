#pragma once

#include "esphome/core/component.h"
#include <vector>
#include <string>

namespace esphome {
namespace storage {

class StorageFile : public Component {
 public:
  void set_path(const std::string &path) { path_ = path; }
  std::string get_path() const { return path_; }
  std::vector<uint8_t> read();  // Déclaration manquante <button class="citation-flag" data-index="3">

 private:
  std::string path_;
};

class StorageComponent : public Component {
 public:
  void set_platform(const std::string &platform) { platform_ = platform; }
  void setup() override;  // Déclaration sans définition inline <button class="citation-flag" data-index="2">
  void on_setup_web_server();  // Déclaration manquante <button class="citation-flag" data-index="3">
  
  void add_file(StorageFile *file) { files_.push_back(file); }

 protected:
  void setup_sd_card();   // Déclarations manquantes <button class="citation-flag" data-index="3">
  void setup_flash();
  void setup_inline();
  static esp_err_t serve_file_handler(httpd_req_t *req);  // Déclaration statique <button class="citation-flag" data-index="4">

 private:
  std::string platform_;
  std::vector<StorageFile*> files_;
};

}  // namespace storage
}  // namespace esphome











