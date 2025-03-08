#pragma once

#include "esphome/core/component.h"
#include "esphome/audio/audio_file.h"  // Inclusion obligatoire
#include "esp_http_server.h"
#include <vector>
#include <string>

namespace esphome {
namespace storage {

class StorageFile : public audio::AudioFile, public Component {  // Héritage double
 public:
  void set_path(const std::string &path) override {  // Override explicite
    path_ = path;
  }

  std::string get_path() const override {  // Override explicite
    return path_;
  }

  std::vector<uint8_t> read() override;  // Implémentation obligatoire

  void set_platform(const std::string &platform) {
    platform_ = platform;
  }

 private:
  std::string path_;
  std::string platform_;
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












