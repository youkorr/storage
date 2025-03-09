#pragma once

#include "esp_http_server.h"
#include "esp_log.h"
#include <vector>
#include <string>

namespace esphome {
namespace storage {

class StorageComponent;  // Déclaration anticipée

class StorageFile {
 public:
  StorageFile(StorageComponent *parent) : parent_(parent) {}
  void set_path(const std::string &path) { path_ = path; }
  std::string get_path() const { return path_; }

  std::vector<uint8_t> read();

 private:
  std::string path_;
  StorageComponent *parent_;
};

class StorageComponent {
 public:
  void set_platform(const std::string &platform) { platform_ = platform; }
  void set_web_server(httpd_handle_t server) { server_ = server; }
  
  void add_file(StorageFile *file) {
    files_.push_back(file);
  }
  
  void setup();
  void on_setup_web_server();

  std::string get_platform() const { return platform_; }

 protected:
  void setup_sd_card();
  void setup_flash();
  void setup_inline();
  esp_err_t serve_file(httpd_req_t *req);

 private:
  std::string platform_;
  httpd_handle_t server_{nullptr};
  std::vector<StorageFile*> files_;
};

}  // namespace storage
}  // namespace esphome











