#include "storage.h"

#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"
#include "esphome/core/component.h"

namespace esphome {
namespace storage {

static const char *const TAG = "storage";

FileInfo::FileInfo(std::string const &path, size_t size, bool is_directory)
    : path(path), size(size), is_directory(is_directory) {
  this->read_offset = 0;
}

FileInfo::FileInfo() : path(), size(), is_directory() { 
  this->read_offset = 0; 
}

// Méthodes de la classe Storage (base)
std::vector<FileInfo> Storage::list_directory(const std::string &path) { 
  return this->direct_list_directory(path); 
}

FileInfo Storage::get_file_info(const std::string &path) { 
  return this->direct_get_file_info(path); 
}

void Storage::set_file(FileInfo *file) {
  if (this->current_file_ != file) {
    this->current_file_ = file;
    this->direct_set_file(this->current_file_->path);
  }
}

uint8_t Storage::read() {
  uint8_t data;
  data = this->direct_read_byte(this->current_file_->read_offset);
  this->update_offset(1);
  return data;
}

bool Storage::write(uint8_t data) {
  ESP_LOGW(TAG, "Buffer not available using direct access instead");
  return this->direct_write_byte(data);
}

bool Storage::append(uint8_t data) { 
  return this->direct_append_byte(data); 
}

size_t Storage::read_array(uint8_t *data, size_t data_length) {
  size_t num_bytes_read = this->direct_read_byte_array(this->current_file_->read_offset, data, data_length);
  this->update_offset(num_bytes_read);
  return num_bytes_read;
}

bool Storage::write_array(uint8_t *data, size_t data_length) {
  return this->direct_write_byte_array(data, data_length);
}

bool Storage::append_array(uint8_t *data, size_t data_length) {
  return this->direct_append_byte_array(data, data_length);
}

void Storage::update_offset(size_t value) {
  this->current_file_->read_offset += value;
}

// Implémentation concrète pour SDStorage
uint8_t SDStorage::direct_read_byte(size_t offset) {
  // TODO: Implémentation pour lire depuis la carte SD
  ESP_LOGD(TAG, "Reading byte at offset %zu from %s", offset, current_file_path_.c_str());
  return 0; // Placeholder
}

bool SDStorage::direct_write_byte(uint8_t data) {
  // TODO: Implémentation pour écrire sur la carte SD
  ESP_LOGD(TAG, "Writing byte 0x%02X to %s", data, current_file_path_.c_str());
  return true; // Placeholder
}

bool SDStorage::direct_append_byte(uint8_t data) {
  // TODO: Implémentation pour ajouter un byte sur la carte SD
  ESP_LOGD(TAG, "Appending byte 0x%02X to %s", data, current_file_path_.c_str());
  return true; // Placeholder
}

size_t SDStorage::direct_read_byte_array(size_t offset, uint8_t *data, size_t data_length) {
  // TODO: Implémentation pour lire un tableau depuis la carte SD
  ESP_LOGD(TAG, "Reading %zu bytes at offset %zu from %s", data_length, offset, current_file_path_.c_str());
  return 0; // Placeholder
}

bool SDStorage::direct_write_byte_array(uint8_t *data, size_t data_length) {
  // TODO: Implémentation pour écrire un tableau sur la carte SD
  ESP_LOGD(TAG, "Writing %zu bytes to %s", data_length, current_file_path_.c_str());
  return true; // Placeholder
}

bool SDStorage::direct_append_byte_array(uint8_t *data, size_t data_length) {
  // TODO: Implémentation pour ajouter un tableau sur la carte SD
  ESP_LOGD(TAG, "Appending %zu bytes to %s", data_length, current_file_path_.c_str());
  return true; // Placeholder
}

void SDStorage::direct_set_file(const std::string &file) {
  this->current_file_path_ = this->path_prefix_ + "/" + file;
  ESP_LOGD(TAG, "Set current file to: %s", this->current_file_path_.c_str());
}

FileInfo SDStorage::direct_get_file_info(const std::string &path) {
  // TODO: Implémentation pour obtenir les infos du fichier depuis la carte SD
  std::string full_path = this->path_prefix_ + "/" + path;
  ESP_LOGD(TAG, "Getting file info for: %s", full_path.c_str());
  return FileInfo(path, 0, false); // Placeholder
}

std::vector<FileInfo> SDStorage::direct_list_directory(const std::string &path) {
  // TODO: Implémentation pour lister le répertoire depuis la carte SD
  std::string full_path = this->path_prefix_ + "/" + path;
  ESP_LOGD(TAG, "Listing directory: %s", full_path.c_str());
  return std::vector<FileInfo>(); // Placeholder
}

// Méthodes StorageClient (inchangées)
std::vector<FileInfo> StorageClient::list_directory(const std::string &path) {
  int prefix_end = path.find("://");
  if (prefix_end < 0) {
    ESP_LOGE(TAG, "Invalid path. Must start with a valid prefix");
    return std::vector<FileInfo>();
  }
  std::string prefix = path.substr(0, prefix_end);
  auto nstorage = storages.find(prefix);
  if (nstorage == storages.end()) {
    ESP_LOGE(TAG, "storage %s prefix does not exist", prefix.c_str());
    return std::vector<FileInfo>();
  }
  std::vector<FileInfo> result = nstorage->second->list_directory(path.substr(prefix_end + 3));
  for (auto i = result.begin(); i != result.end(); i++) {
    i->path = prefix + "://" + i->path;
  }
  return result;
}

FileInfo StorageClient::get_file_info(const std::string &path) {
  int prefix_end = path.find("://");
  if (prefix_end < 0) {
    ESP_LOGE(TAG, "Invalid path. Must start with a valid prefix");
    return FileInfo();
  }
  std::string prefix = path.substr(0, prefix_end);
  auto nstorage = storages.find(prefix);
  if (nstorage == storages.end()) {
    ESP_LOGE(TAG, "storage %s prefix does not exist", prefix.c_str());
    return FileInfo();
  }
  FileInfo result = nstorage->second->get_file_info(path.substr(prefix_end + 3));
  result.path = prefix + "://" + result.path;
  return result;
}

void StorageClient::set_file(const std::string &path) {
  int prefix_end = path.find("://");
  if (prefix_end < 0) {
    ESP_LOGE(TAG, "Invalid path. Must start with a valid prefix");
    return;
  }
  std::string prefix = path.substr(0, prefix_end);
  auto nstorage = storages.find(prefix);
  if (nstorage == storages.end()) {
    ESP_LOGE(TAG, "storage %s prefix does not exist", prefix.c_str());
    return;
  }
  this->current_storage_ = nstorage->second;
  this->current_file_ = this->current_storage_->get_file_info(path.substr(prefix_end + 3));
  this->current_storage_->set_file(&(this->current_file_));
  ESP_LOGVV(TAG, "Current File Set to %s", this->current_file_.path.c_str());
}

uint8_t StorageClient::read() {
  if (this->current_storage_) {
    ESP_LOGVV(TAG, "Reading File: %s", this->current_file_.path.c_str());
    this->current_storage_->set_file(&(this->current_file_));
    return current_storage_->read();
  } else {
    ESP_LOGE(TAG, "File has not been set");
    return 0;
  }
}

void StorageClient::set_read_offset(size_t offset) {
  if (this->current_storage_) {
    this->current_file_.read_offset = offset;
  } else {
    ESP_LOGE(TAG, "File has not been set");
  }
}

bool StorageClient::write(uint8_t data) {
  if (current_storage_) {
    this->current_storage_->set_file(&(this->current_file_));
    return current_storage_->write(data);
  } else {
    ESP_LOGE(TAG, "File has not been set");
    return false;
  }
}

bool StorageClient::append(uint8_t data) {
  if (current_storage_) {
    this->current_storage_->set_file(&(this->current_file_));
    return current_storage_->append(data);
  } else {
    ESP_LOGE(TAG, "File has not been set");
    return false;
  }
}

size_t StorageClient::read_array(uint8_t *data, size_t data_length) {
  if (current_storage_) {
    ESP_LOGVV(TAG, "Reading File: %s", this->current_file_.path.c_str());
    this->current_storage_->set_file(&(this->current_file_));
    return current_storage_->read_array(data, data_length);
  } else {
    ESP_LOGE(TAG, "File has not been set");
    return 0;
  }
}

bool StorageClient::write_array(uint8_t *data, size_t data_length) {
  if (current_storage_) {
    this->current_storage_->set_file(&(this->current_file_));
    return current_storage_->write_array(data, data_length);
  } else {
    ESP_LOGE(TAG, "File has not been set");
    return false;
  }
}

bool StorageClient::append_array(uint8_t *data, size_t data_length) {
  if (current_storage_) {
    this->current_storage_->set_file(&(this->current_file_));
    return current_storage_->append_array(data, data_length);
  } else {
    ESP_LOGE(TAG, "File has not been set");
    return false;
  }
}

std::map<std::string, Storage *> StorageClient::storages = {};

void StorageClient::add_storage(Storage *storage_inst, std::string prefix) {
  StorageClient::storages[prefix] = storage_inst;
}

}  // namespace storage
}  // namespace esphome











