#pragma once

#include "esphome/core/component.h"
#include "esphome/core/entity_base.h"
#include <vector>
#include <map>

namespace esphome {
namespace storage {

struct FileInfo {
  std::string path;
  size_t size;
  bool is_directory;
  size_t read_offset;
  FileInfo(std::string const &path, size_t size, bool is_directory);
  FileInfo();
};

class Storage : public Component {  // ← CHANGÉ : EntityBase → Component
 public:
  // Méthodes Component obligatoires
  void setup() override {}
  void dump_config() override {}
  
  // Méthodes pour la configuration depuis Python
  void set_path_prefix(const std::string &prefix) { this->path_prefix_ = prefix; }
  void set_sd_mmc_card(void* sd_mmc) { this->sd_mmc_card_ = sd_mmc; }
  
  // Méthodes virtuelles pures (doivent être implémentées par les classes filles)
  virtual uint8_t direct_read_byte(size_t offset) = 0;
  virtual bool direct_write_byte(uint8_t data) = 0;
  virtual bool direct_append_byte(uint8_t data) = 0;
  virtual size_t direct_read_byte_array(size_t offset, uint8_t *data, size_t data_length) = 0;
  virtual bool direct_write_byte_array(uint8_t *data, size_t data_length) = 0;
  virtual bool direct_append_byte_array(uint8_t *data, size_t data_length) = 0;
  
  // Méthodes publiques
  std::vector<FileInfo> list_directory(const std::string &path);
  FileInfo get_file_info(const std::string &path);
  void set_file(FileInfo *file);
  uint8_t read();
  bool write(uint8_t data);
  bool append(uint8_t data);
  size_t read_array(uint8_t *data, size_t data_length);
  bool write_array(uint8_t *data, size_t data_length);
  bool append_array(uint8_t *data, size_t data_length);

 protected:
  virtual void direct_set_file(const std::string &file) = 0;
  virtual FileInfo direct_get_file_info(const std::string &path) = 0;
  virtual std::vector<FileInfo> direct_list_directory(const std::string &path) = 0;
  void update_offset(size_t value);
  FileInfo *current_file_{nullptr};
  
  // Membres pour la configuration
  std::string path_prefix_;
  void* sd_mmc_card_{nullptr};
};

// Classe concrète pour SD Storage
class SDStorage : public Storage {
 public:
  // Implémentation des méthodes virtuelles pures
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

 private:
  std::string current_file_path_;
};

class StorageClient : public EntityBase {
 public:
  std::vector<FileInfo> list_directory(const std::string &path);
  FileInfo get_file_info(const std::string &path);
  void set_file(const std::string &path);
  uint8_t read();
  void set_read_offset(size_t offset);
  bool write(uint8_t data);
  bool append(uint8_t data);
  size_t read_array(uint8_t *data, size_t data_length);
  bool write_array(uint8_t *data, size_t data_length);
  bool append_array(uint8_t *data, size_t data_length);

  static void add_storage(Storage *storage_inst, std::string prefix);

 protected:
  static std::map<std::string, Storage *> storages;
  Storage *current_storage_{nullptr};
  FileInfo current_file_;
};

}  // namespace storage
}  // namespace esphome





















