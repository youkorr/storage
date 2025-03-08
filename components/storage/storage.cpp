#include "storage.h"
#include "esphome/core/log.h"
#include "esphome/components/web_server_base/web_server_base.h"
#include <fstream>

namespace esphome {
namespace storage {

static const char *const TAG = "storage";

std::vector<uint8_t> StorageComponent::read_file(char const *path) {
  return read_file(std::string(path));
}

std::vector<uint8_t> StorageComponent::read_file(std::string const &path) {
  std::vector<uint8_t> data;
  
  if (platform_ == "sd_card") {
    // Implementation for SD card
    std::ifstream file(path, std::ios::binary);
    if (file) {
      file.seekg(0, std::ios::end);
      data.resize(file.tellg());
      file.seekg(0, std::ios::beg);
      file.read(reinterpret_cast<char*>(data.data()), data.size());
    }
  } else if (platform_ == "flash") {
    // Implementation for flash storage
    // This would depend on your specific flash storage implementation
  } else if (platform_ == "inline") {
    // Implementation for inline storage
    // This would depend on your specific inline storage implementation
  }

  if (data.empty()) {
    ESP_LOGE(TAG, "Failed to read file: %s", path.c_str());
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
  for (const auto &file : files_) {
    std::string url = "/media/" + file.second;
    web_server_->add_handler(url, [this, file](web_server_base::Request *req) {
      this->serve_file(file.first, file.second);
    });
    ESP_LOGD(TAG, "Registered media URL: %s -> %s", url.c_str(), file.first.c_str());
  }
}

void StorageComponent::serve_file(const std::string &path, const std::string &id) {
  auto data = read_file(path);
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



