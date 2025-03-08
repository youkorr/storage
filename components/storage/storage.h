#pragma once

#include "esphome/core/component.h"
#include <ESPAsyncWebServer.h>  // Inclusion corrigée
#include <vector>
#include <string>

namespace esphome {
namespace storage {

class StorageFile : public Component {
 public:
  void set_path(const std::string &path) { path_ = path; }
  std::string get_path() const { return path_; }
  std::vector<uint8_t> read();

 private:
  std::string path_;
};

class StorageComponent : public Component {
 public:
  void set_platform(const std::string &platform) { platform_ = platform; }
  void set_web_server(AsyncWebServer *web_server) { web_server_ = web_server; }
  
  void add_file(StorageFile *file) {
    files_.push_back(file);
  }
  
  void setup() override;
  void on_setup_web_server();

 protected:
  void setup_sd_card();
  void setup_flash();
  void setup_inline();
  void serve_file(StorageFile *file, AsyncWebServerRequest *req);

 private:
  std::string platform_;
  AsyncWebServer *web_server_{nullptr};
  std::vector<StorageFile*> files_;
};

}  // namespace storage
}  // namespace esphome












