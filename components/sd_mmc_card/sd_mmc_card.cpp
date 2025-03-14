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

void SdMmc::loop() {}

void SdMmc::dump_config() {
  ESP_LOGCONFIG(TAG, "SD MMC Component");
}

void SdMmc::write_file(const char *path, const uint8_t *buffer, size_t len) {
  ESP_LOGV(TAG, "Writing to file: %s", path);
}

void SdMmc::append_file(const char *path, const uint8_t *buffer, size_t len) {
  ESP_LOGV(TAG, "Appending to file: %s", path);
}

#ifndef USE_ESP_IDF
void SdMmc::write_file_chunked(const char *path, const uint8_t *buffer, size_t len, size_t chunk_size) {
}
#endif

std::vector<std::string> SdMmc::list_directory(const char *path, uint8_t depth) {
    return {};
}

std::vector<std::string> SdMmc::list_directory(std::string path, uint8_t depth) {
    return {};
}

std::vector<FileInfo> SdMmc::list_directory_file_info(const char *path, uint8_t depth) {
    return {};
}

std::vector<FileInfo> SdMmc::list_directory_file_info(std::string path, uint8_t depth) {
    return {};
}

size_t SdMmc::file_size(std::string const &path) { return {}; }

bool SdMmc::is_directory(std::string const &path) { return {}; }

bool SdMmc::delete_file(std::string const &path) { return {}; }

std::vector<uint8_t> SdMmc::read_file(std::string const &path) { return {}; }

std::vector<uint8_t> SdMmc::read_file_chunked(std::string const &path, size_t offset, size_t chunk_size) {
    return {};
}

#ifdef USE_SENSOR
void SdMmc::add_file_size_sensor(sensor::Sensor *sensor, std::string const &path) {}
#endif

void SdMmc::set_clk_pin(uint8_t pin) {}

void SdMmc::set_cmd_pin(uint8_t pin) {}

void SdMmc::set_data0_pin(uint8_t pin) {}

void SdMmc::set_data1_pin(uint8_t pin) {}

void SdMmc::set_data2_pin(uint8_t pin) {}

void SdMmc::set_data3_pin(uint8_t pin) {}

void SdMmc::set_mode_1bit(bool b) {}

void SdMmc::set_power_ctrl_pin(GPIOPin *pin) {}

long double convertBytes(uint64_t value, MemoryUnit unit) {
    static constexpr uint64_t factors[] = {1ULL, 1024ULL, 1024ULL * 1024ULL, 1024ULL * 1024ULL * 1024ULL};
    if (static_cast<int>(unit) < static_cast<int>(MemoryUnit::BYTES)
        || static_cast<int>(unit) > static_cast<int>(MemoryUnit::GIGABYTES)) {
        ESP_LOGE(TAG, "Invalid memory unit provided");
        return -1.0;
    }
    return static_cast<long double>(value) / factors[static_cast<int>(unit)];
}

FileInfo::FileInfo(std::string const &path, size_t size, bool is_directory)
    : path(path), size(size), is_directory(is_directory) {}

} // namespace sd_mmc_card
} // namespace esphome





