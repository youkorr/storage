#include "storage.h"
#include "esphome/core/log.h"
#include "esphome/components/web_server_base/web_server_base.h"
#include <fstream>

namespace esphome {
namespace storage {

static const char *const TAG = "storage";

std::vector<uint8_t> StorageComponent::read_file(StorageFile *file) {
  return read_file(file->get_path());
}

void StorageComponent::setup() {
  ESP_LOGD(TAG, "Setting up storage platform: %s", platform_.c_str());
  
  if (platform_ == "sd_card") {
    setup_sd_card();
  } else if (platform_ == "flash") {
    setup_flash();
  } else if (platform_ == "inline") {
    setup_inline();
  }

  if (web_server_ != nullptr) {
    this->on_setup_web_server();
  }
}

void StorageComponent::on_setup_web_server() {
  for (auto *file : files_) {
    std::string url = "/media/" + file->get_path();
    web_server_->add_handler(url, [this, file](web_server_base::Request *req) {
      this->serve_file(file);
    });
    ESP_LOGD(TAG, "Registered media URL: %s", url.c_str());
  }
}

void StorageComponent::serve_file(StorageFile *file) {
  auto data = read_file(file);
  if (!data.empty()) {
    web_server_->send_data(req, data.data(), data.size(), "audio/mpeg");
  } else {
    web_server_->send_error(req, 404, "File not found");
  }
}

void StorageComponent::setup_sd_card() {
  ESP_LOGD(TAG, "Initializing SD card storage");
}

void StorageComponent::setup_flash() {
  ESP_LOGD(TAG, "Initializing flash storage");
}

void StorageComponent::setup_inline() {
  ESP_LOGD(TAG, "Initializing inline storage");
}

}  // namespace storage
}  // namespace esphome



