#pragma once

#include "esphome/core/component.h"
#include "esp_http_server.h"
#include <vector>
#include <string>

namespace esphome {
namespace storage {

class StorageFile : public Component {
 public:
  void set_path(const std::string &path) { path_ = path; }
  void set_platform(const std::string &platform) { platform_ = platform; }  // Ajouté
  std::string get_path() const { return path_; }
  std::vector<uint8_t> read();

 private:
  std::string path_;
  std::string platform_;  // Ajouté
};

class StorageComponent : public Component {
 public:
  void set_platform(const std::string &platform) { platform_ = platform; }
  void setup_web_server(httpd_handle_t server);
  
  void add_file(StorageFile *file) {
    files_.push_back(file);
  }
  
  void setup() override;
  void on_setup_web_server();

 protected:
  void setup_sd_card();
  void setup_flash();
  void setup_inline();
  static esp_err_t serve_file_handler(httpd_req_t *req);

 private:
  std::string platform_;
  httpd_handle_t server_{nullptr};
  std::vector<StorageFile*> files_;
};

}  // namespace storage
}  // namespace esphome












