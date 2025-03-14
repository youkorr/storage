#include "sd_mmc_card.h"

#include <algorithm>
#include <vector>
#include <cstdio>

#include "math.h"
#include "esphome/core/log.h"

namespace esphome {
namespace sd_mmc_card {

static const char *TAG = "sd_mmc_card";

#ifdef USE_SENSOR
FileSizeSensor::FileSizeSensor(sensor::Sensor *sensor, std::string const &path) : sensor(sensor), path(path) {}
#endif

#ifndef USE_ESP_IDF
void SdMmc::setup() {}

void SdMmc::loop() {}

void SdMmc::dump_config() {
  ESP_LOGCONFIG(TAG, "SD MMC Component");
  ESP_LOGCONFIG(TAG, "  Mode 1 bit: %s", TRUEFALSE(this->mode_1bit_));
  ESP_LOGCONFIG(TAG, "  CLK Pin: %d", this->clk_pin_);
  ESP_LOGCONFIG(TAG, "  CMD Pin: %d", this->cmd_pin_);
  ESP_LOGCONFIG(TAG, "  DATA0 Pin: %d", this->data0_pin_);
  if (!this->mode_1bit_) {
    ESP_LOGCONFIG(TAG, "  DATA1 Pin: %d", this->data1_pin_);
    ESP_LOGCONFIG(TAG, "  DATA2 Pin: %d", this->data2_pin_);
    ESP_LOGCONFIG(TAG, "  DATA3 Pin: %d", this->data3_pin_);
  }

  if (this->power_ctrl_pin_ != nullptr) {
    LOG_PIN("  Power Ctrl Pin: ", this->power_ctrl_pin_);
  }

#ifdef USE_SENSOR
  LOG_SENSOR("  ", "Used space", this->used_space_sensor_);
  LOG_SENSOR("  ", "Total space", this->total_space_sensor_);
  LOG_SENSOR("  ", "Free space", this->free_space_sensor_);
  for (auto &sensor : this->file_size_sensors_) {
    if (sensor.sensor != nullptr)
      LOG_SENSOR("  ", "File size", sensor.sensor);
  }
#endif
#ifdef USE_TEXT_SENSOR
  LOG_TEXT_SENSOR("  ", "SD Card Type", this->sd_card_type_text_sensor_);
#endif

  if (this->is_failed()) {
    ESP_LOGE(TAG, "Setup failed : %s", SdMmc::error_code_to_string(this->init_error_).c_str());
    return;
  }
}

void SdMmc::write_file(const char *path, const uint8_t *buffer, size_t len, const char *mode) {
  ESP_LOGW(TAG, "Non-ESP-IDF write_file not implemented");
}

void SdMmc::write_file(const char *path, const uint8_t *buffer, size_t len) {
  this->write_file(path, buffer, len, "w");
}

void SdMmc::append_file(const char *path, const uint8_t *buffer, size_t len) {
  this->write_file(path, buffer, len, "a");
}

void SdMmc::write_file_chunked(const char *path, const uint8_t *buffer, size_t len, size_t chunk_size) {
  ESP_LOGW(TAG, "Non-ESP-IDF write_file_chunked not implemented");
}

bool SdMmc::delete_file(const char *path) {
  ESP_LOGW(TAG, "Non-ESP-IDF delete_file not implemented");
  return false;
}

bool SdMmc::create_directory(const char *path) {
  ESP_LOGW(TAG, "Non-ESP-IDF create_directory not implemented");
  return false;
}

bool SdMmc::remove_directory(const char *path) {
  ESP_LOGW(TAG, "Non-ESP-IDF remove_directory not implemented");
  return false;
}

std::vector<uint8_t> SdMmc::read_file(char const *path) {
    ESP_LOGW(TAG, "Non-ESP-IDF read_file not implemented");
    return {};
}

bool SdMmc::is_directory(const char *path) {
    ESP_LOGW(TAG, "Non-ESP-IDF is_directory not implemented");
    return false;
}

std::vector<std::string> SdMmc::list_directory(const char *path, uint8_t depth) {
    ESP_LOGW(TAG, "Non-ESP-IDF list_directory not implemented");
    return {};
}

std::vector<FileInfo> SdMmc::list_directory_file_info(const char *path, uint8_t depth) {
    ESP_LOGW(TAG, "Non-ESP-IDF list_directory_file_info not implemented");
    return {};
}

size_t SdMmc::file_size(const char *path) {
    ESP_LOGW(TAG, "Non-ESP-IDF file_size not implemented");
    return 0;
}

std::vector<uint8_t> SdMmc::read_file_chunked(char const *path, size_t offset, size_t chunk_size) {
    ESP_LOGW(TAG, "Non-ESP-IDF read_file_chunked not implemented");
    return {};
}

void SdMmc::update_sensors() {
  ESP_LOGW(TAG, "Non-ESP-IDF update_sensors not implemented");
}

#ifdef USE_ESP32_FRAMEWORK_ARDUINO
std::string SdMmc::sd_card_type_to_string(int type) const {
    ESP_LOGW(TAG, "Non-ESP-IDF sd_card_type_to_string not implemented");
    return {};
}
#endif

std::vector<FileInfo> &SdMmc::list_directory_file_info_rec(const char *path, uint8_t depth, std::vector<FileInfo> &list) {
    ESP_LOGW(TAG, "Non-ESP-IDF list_directory_file_info_rec not implemented");
    return list;
}
#endif

void SdMmc::set_clk_pin(uint8_t pin) { this->clk_pin_ = pin; }

void SdMmc::set_cmd_pin(uint8_t pin) { this->cmd_pin_ = pin; }

void SdMmc::set_data0_pin(uint8_t pin) { this->data0_pin_ = pin; }

void SdMmc::set_data1_pin(uint8_t pin) { this->data1_pin_ = pin; }

void SdMmc::set_data2_pin(uint8_t pin) { this->data2_pin_ = pin; }

void SdMmc::set_data3_pin(uint8_t pin) { this->data3_pin_ = pin; }

void SdMmc::set_mode_1bit(bool b) { this->mode_1bit_ = b; }

void SdMmc::set_power_ctrl_pin(GPIOPin *pin) { this->power_ctrl_pin_ = pin; }

std::string SdMmc::error_code_to_string(ErrorCode code) {
  switch (code) {
    case ErrorCode::ERR_PIN_SETUP:
      return "Failed to set pins";
    case ErrorCode::ERR_MOUNT:
      return "Failed to mount card";
    case ErrorCode::ERR_NO_CARD:
      return "No card found";
    default:
      return "Unknown error";
  }
}

long double convertBytes(uint64_t value, MemoryUnits unit) {
    static constexpr uint64_t factors[] = {1ULL, 1024ULL, 1024ULL * 1024ULL,
                                           1024ULL * 1024ULL * 1024ULL,
                                           1024ULL * 1024ULL * 1024ULL * 1024ULL,
                                           static_cast<uint64_t>(1024ULL) *
                                               static_cast<uint64_t>(1024ULL) *
                                               static_cast<uint64_t>(1024ULL) *
                                               static_cast<uint64_t>(1024ULL) *
                                               static_cast<uint64_t>(1024ULL)};

    if (unit < MemoryUnits::Byte || unit > MemoryUnits::PetaByte) {
        ESP_LOGE(TAG, "Invalid memory unit provided");
        return -1.0;
    }

    return static_cast<long double>(value) / factors[unit];
}

FileInfo::FileInfo(std::string const &path, size_t size, bool is_directory)
    : path(path), size(size), is_directory(is_directory) {}

#ifdef USE_SENSOR
FileSizeSensor::FileSizeSensor(sensor::Sensor *sensor, std::string const &path) : sensor(sensor), path(path) {}
#endif

}  // namespace sd_mmc_card
}  // namespace esphome







