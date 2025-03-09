#include "storage.h"
#include <fstream>
#include <cstring>

namespace esphome {
namespace storage {

static const char *TAG = "storage";

std::vector<uint8_t> StorageFile::read() {
  std::vector<uint8_t> data;
  
  if (parent_->get_platform() == "sd_card") {
    std::ifstream file(path_, std::ios::binary);
    if (file) {
      file.seekg(0, std::ios::end);
      data.resize(file.tellg());
      file.seekg(0, std::ios::beg);
      file.read(reinterpret_cast<char*>(data.data()), data.size());
    }
  } else if (parent_->get_platform() == "flash") {
    // Implementation for flash storage
  } else if (parent_->get_platform() == "inline") {
    // Implementation for inline storage
  }

  if (data.empty()) {
    ESP_LOGE(TAG, "Failed to read file: %s", path_.c_str());
  }

  return data;
}

void StorageComponent::setup() {
  ESP_LOGI(TAG, "Setting up storage platform: %s", platform_.c_str());
  
  if (platform_ == "sd_card") {
    setup_sd_card();
  } else if (platform_ == "flash") {
    setup_flash();
  } else if (platform_ == "inline") {
    setup_inline();
  }

  if (server_ != nullptr) {
    this->on_setup_web_server();
  }
}

void StorageComponent::on_setup_web_server() {
  for (auto *file : files_) {
    std::string url = "/media/" + file->get_path();
    httpd_uri_t uri = {
      .uri = url.c_str(),
      .method = HTTP_GET,
      .handler = [](httpd_req_t *req) {
        return reinterpret_cast<StorageComponent*>(req->user_ctx)->serve_file(req);
      },
      .user_ctx = this
    };
    httpd_register_uri_handler(server_, &uri);
    ESP_LOGI(TAG, "Registered media URL: %s", url.c_str());
  }
}

esp_err_t StorageComponent::serve_file(httpd_req_t *req) {
  std::string path = req->uri + 7; // Remove "/media/" prefix
  for (auto *file : files_) {
    if (file->get_path() == path) {
      auto data = file->read();
      if (!data.empty()) {
        httpd_resp_set_type(req, "audio/mpeg");
        httpd_resp_send(req, reinterpret_cast<const char*>(data.data()), data.size());
        return ESP_OK;
      }
    }
  }
  httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
  return ESP_FAIL;
}

void StorageComponent::setup_sd_card() {
  ESP_LOGI(TAG, "Initializing SD card storage");
}

void StorageComponent::setup_flash() {
  ESP_LOGI(TAG, "Initializing flash storage");
}

void StorageComponent::setup_inline() {
  ESP_LOGI(TAG, "Initializing inline storage");
}

}  // namespace storage
}  // namespace esphome



