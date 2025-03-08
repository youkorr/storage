#include "storage.h"
#include "esphome/core/log.h"
#include "esphome/components/web_server_base/web_server_base.h"
#include <fstream>

namespace esphome {
namespace storage {

static const char *const TAG = "storage";

std::vector<uint8_t> StorageFile::read() {
  std::vector<uint8_t> data;
  
  if (platform_ == "sd_card") {
    std::ifstream file(path_, std::ios::binary);
    if (file) {
      file.seekg(0, std::ios::end);
      data.resize(file.tellg());
      file.seekg(0, std::ios::beg);
      file.read(reinterpret_cast<char*>(data.data()), data.size());
    }
  } else if (platform_ == "flash") {
    // Implementation for flash storage
  } else if (platform_ == "inline") {
    // Implementation for inline storage
  }

  if (data.empty()) {
    ESP_LOGE(TAG, "Failed to read file: %s", path_.c_str());
  }

  return data;
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
      this->serve_file(file, req);
    });
    ESP_LOGD(TAG, "Registered media URL: %s", url.c_str());
  }
}

void StorageComponent::serve_file(StorageFile *file, web_server_base::Request *req) {
  auto data = file->read();
  if (!data.empty()) {
    req->send(200, "audio/mpeg", data.data(), data.size());
  } else {
    req->send(404, "text/plain", "File not found");
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



