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
  // Implémentation à ajouter
}

void SdMmc::write_file(const char *path, const uint8_t *buffer, size_t len) {
  this->write_file(path, buffer, len, "w");
}

void SdMmc::append_file(const char *path, const uint8_t *buffer, size_t len) {
  this->write_file(path, buffer, len, "a");
}

void SdMmc::write_file_chunked(const char *path, const uint8_t *buffer, size_t len, size_t chunk_size) {
  // Implémentation à ajouter
}

bool SdMmc::delete_file(const char *path) {
  // Implémentation à ajouter
  return false;
}

bool SdMmc::delete_file(std::string const &path) {
  return this->delete_file(path.c_str());
}

bool SdMmc::create_directory(const char *path) {
  // Implémentation à ajouter
  return false;
}

bool SdMmc::remove_directory(const char *path) {
  // Implémentation à ajouter
  return false;
}

std::vector<uint8_t> SdMmc::read_file(char const *path) {
  // Implémentation à ajouter
  return {};
}

std::vector<uint8_t> SdMmc::read_file(std::string const &path) {
  return this->read_file(path.c_str());
}

std::vector<uint8_t> SdMmc::read_file_chunked(char const *path, size_t offset, size_t chunk_size) {
  // Implémentation à ajouter
  return {};
}

std::vector<uint8_t> SdMmc::read_file_chunked(std::string const &path, size_t offset, size_t chunk_size) {
  return this->read_file_chunked(path.c_str(), offset, chunk_size);
}

bool SdMmc::is_directory(const char *path) {
  // Implémentation à ajouter
  return false;
}

bool SdMmc::is_directory(std::string const &path) {
  return this->is_directory(path.c_str());
}

std::vector<std::string> SdMmc::list_directory(const char *path, uint8_t depth) {
  // Implémentation à ajouter
  return {};
}

std::vector<std::string> SdMmc::list_directory(std::string path, uint8_t depth) {
  return this->list_directory(path.c_str(), depth);
}

std::vector<FileInfo> SdMmc::list_directory_file_info(const char *path, uint8_t depth) {
  // Implémentation à ajouter
  return {};
}

std::vector<FileInfo> SdMmc::list_directory_file_info(std::string path, uint8_t depth) {
  return this->list_directory_file_info(path.c_str(), depth);
}

size_t SdMmc::file_size(const char *path) {
  // Implémentation à ajouter
  return 0;
}

size_t SdMmc::file_size(std::string const &path) {
  return this->file_size(path.c_str());
}

#ifdef USE_SENSOR
void SdMmc::add_file_size_sensor(sensor::Sensor *sensor, std::string const &path) {
  this->file_size_sensors_.emplace_back(sensor, path);
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

void SdMmc::update_sensors() {
  // Implémentation à ajouter
}

#ifdef USE_ESP32_FRAMEWORK_ARDUINO
std::string SdMmc::sd_card_type_to_string(int type) const {
  // Implémentation à ajouter
  return "";
}
#endif

#ifdef USE_ESP_IDF
std::string SdMmc::sd_card_type() const {
  // Implémentation à ajouter
  return "";
}
#endif

std::vector<FileInfo> &SdMmc::list_directory_file_info_rec(const char *path, uint8_t depth, std::vector<FileInfo> &list) {
  // Implémentation à ajouter
  return list;
}

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
                                         1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL};
  
  if (static_cast<int>(unit) < 0 || static_cast<int>(unit) > static_cast<int>(MemoryUnits::PetaByte)) {
    ESP_LOGE(TAG, "Invalid memory unit provided");
    return -1.0;
  }

  return static_cast<long double>(value) / factors[static_cast<int>(unit)];
}

FileInfo::FileInfo(std::string const &path, size_t size, bool is_directory)
    : path(path), size(size), is_directory(is_directory) {}

}  // namespace sd_mmc_card
}  // namespace esphome






