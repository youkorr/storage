#include "storage.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/components/sd_mmc_card/sd_mmc_card.h"

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
  ESP_LOGD(TAG, "Initializing SD card storage using SD_MMC component");
  
  if (this->sd_card_ == nullptr) {
    ESP_LOGE(TAG, "SD_MMC component not found! Make sure it's configured in your YAML.");
    return;
  }
  
  // Check if SD card is mounted
  if (!this->sd_card_->is_mounted()) {
    ESP_LOGE(TAG, "SD card is not mounted");
    return;
  }
  
  // List root directory
  auto root_files = this->sd_card_->list_directory("/");
  if (root_files.empty()) {
    ESP_LOGW(TAG, "Root directory is empty or cannot be accessed");
  } else {
    ESP_LOGI(TAG, "Found %d files/directories in root:", root_files.size());
    for (const auto &file_info : root_files) {
      ESP_LOGI(TAG, "  %s (%s)", 
               file_info.name.c_str(),
               file_info.is_directory ? "DIR" : "FILE");
    }
  }
  
  // Verify access to registered files
  for (const auto *storage_file : files_) {
    if (storage_file->get_platform() == "sd_card") {
      std::string filepath = storage_file->get_path();
      if (filepath.empty() || filepath[0] != '/') {
        filepath = "/" + filepath;
      }
      
      if (this->sd_card_->exists(filepath.c_str())) {
        if (this->sd_card_->is_directory(filepath.c_str())) {
          ESP_LOGI(TAG, "Directory exists: %s", filepath.c_str());
        } else {
          ESP_LOGI(TAG, "File exists: %s (%d bytes)", 
                   filepath.c_str(),
                   this->sd_card_->file_size(filepath.c_str()));
        }
      } else {
        ESP_LOGW(TAG, "File or directory not found: %s", filepath.c_str());
      }
    }
  }
}

void StorageComponent::setup_flash() {
  ESP_LOGD(TAG, "Initializing flash storage");
}

void StorageComponent::setup_inline() {
  ESP_LOGD(TAG, "Initializing inline storage");
}

}  // namespace storage
}  // namespace esphome







