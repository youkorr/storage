#pragma once

#include "storage.h"
#include "esphome/core/component.h"
#include "esphome/components/sd_mmc_card/sd_mmc_card.h"

namespace esphome {
namespace storage {

class SDStorage : public Storage, public Component {
 public:
  // Méthodes ESPHome Component
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // Configuration depuis __init__.py
  void set_path_prefix(const std::string &prefix) { this->path_prefix_ = prefix; }
  void set_sd_mmc_card(sd_mmc_card::SDMMCCard *sd_mmc) { this->sd_mmc_ = sd_mmc; }

  // Implémentation des méthodes virtuelles de Storage
  uint8_t direct_read_byte(size_t offset) override;
  bool direct_write_byte(uint8_t data) override;
  bool direct_append_byte(uint8_t data) override;
  size_t direct_read_byte_array(size_t offset, uint8_t *data, size_t data_length) override;
  bool direct_write_byte_array(uint8_t *data, size_t data_length) override;
  bool direct_append_byte_array(uint8_t *data, size_t data_length) override;

 protected:
  void direct_set_file(const std::string &file) override;
  FileInfo direct_get_file_info(const std::string &path) override;
  std::vector<FileInfo> direct_list_directory(const std::string &path) override;

  std::string path_prefix_;
  sd_mmc_card::SDMMCCard *sd_mmc_{nullptr};
  std::string current_file_path_;
  FILE *current_file_{nullptr};

  std::string get_full_path(const std::string &path);
  void close_current_file();
  bool ensure_sd_mounted();
};

}  // namespace storage
}  // namespace esphome
