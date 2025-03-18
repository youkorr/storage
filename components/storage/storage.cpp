#include "storage.h"
#include "esphome/core/log.h"

namespace esphome {
namespace storage {

static const char *const TAG = "storage";

std::string StorageComponent::get_file_path(const std::string &file_id) const {
  for (const auto *file : files_) {
    if (file->get_id() == file_id) {
      return file->get_path();
    }
  }
  ESP_LOGW(TAG, "File with ID %s not found", file_id.c_str());
  return "";
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

  for (const auto *file : files_) {
    ESP_LOGD(TAG, "Registered file: %s -> %s (platform: %s)", 
             file->get_path().c_str(), 
             file->get_id().c_str(),
             file->get_platform().c_str());
  }
}

void StorageComponent::setup_sd_card() {
  ESP_LOGD(TAG, "Initializing SD card storage");
  // SD card initialization logic here
}

void StorageComponent::setup_flash() {
  ESP_LOGD(TAG, "Initializing flash storage");
  // Flash storage logic here
}

void StorageComponent::setup_inline() {
  ESP_LOGD(TAG, "Initializing inline storage");
  // Inline storage logic here
}

}  // namespace storage
}  // namespace esphome





