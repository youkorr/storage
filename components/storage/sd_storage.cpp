#include "sd_storage.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include <sys/stat.h>
#include <dirent.h>

namespace esphome {
namespace storage {

static const char *const TAG = "storage.sd";

void SDStorage::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SD Storage...");
  
  // Vérifier que la carte SD est accessible
  if (!this->ensure_sd_mounted()) {
    ESP_LOGE(TAG, "SD card not accessible - will retry during operation");
    // Ne pas marquer comme failed, car la carte peut être montée plus tard
  } else {
    ESP_LOGCONFIG(TAG, "SD Storage ready with prefix: %s", this->path_prefix_.c_str());
  }
}

void SDStorage::dump_config() {
  ESP_LOGCONFIG(TAG, "SD Storage:");
  ESP_LOGCONFIG(TAG, "  Path prefix: %s", this->path_prefix_.c_str());
  
  // Test simple d'accès au répertoire
  struct stat st;
  bool sd_available = (stat("/sdcard", &st) == 0 && S_ISDIR(st.st_mode));
  ESP_LOGCONFIG(TAG, "  SD card accessible: %s", sd_available ? "yes" : "no");
}

bool SDStorage::ensure_sd_mounted() {
  // Vérifier si la carte SD est montée en testant l'accès au répertoire racine
  struct stat st;
  if (stat("/sdcard", &st) == 0 && S_ISDIR(st.st_mode)) {
    return true;
  }
  
  ESP_LOGVV(TAG, "SD card not mounted or not accessible");
  return false;
}

std::string SDStorage::get_full_path(const std::string &path) {
  std::string full_path = "/sdcard";
  if (!path.empty() && path[0] != '/') {
    full_path += "/";
  }
  full_path += path;
  return full_path;
}

void SDStorage::close_current_file() {
  if (this->current_file_ != nullptr) {
    fclose(this->current_file_);
    this->current_file_ = nullptr;
  }
  this->current_file_path_.clear();
}

void SDStorage::direct_set_file(const std::string &file) {
  // Si c'est déjà le fichier courant, ne rien faire
  if (this->current_file_path_ == file && this->current_file_ != nullptr) {
    return;
  }
  
  // Fermer le fichier précédent
  this->close_current_file();
  
  // Ouvrir le nouveau fichier
  std::string full_path = this->get_full_path(file);
  this->current_file_ = fopen(full_path.c_str(), "r+b");
  
  if (this->current_file_ == nullptr) {
    // Essayer de créer le fichier s'il n'existe pas
    this->current_file_ = fopen(full_path.c_str(), "w+b");
  }
  
  if (this->current_file_ != nullptr) {
    this->current_file_path_ = file;
    ESP_LOGVV(TAG, "File opened: %s", full_path.c_str());
  } else {
    ESP_LOGE(TAG, "Failed to open file: %s", full_path.c_str());
  }
}

FileInfo SDStorage::direct_get_file_info(const std::string &path) {
  std::string full_path = this->get_full_path(path);
  
  struct stat st;
  if (stat(full_path.c_str(), &st) != 0) {
    ESP_LOGVV(TAG, "File not found: %s", full_path.c_str());
    return FileInfo(path, 0, false);
  }
  
  bool is_dir = S_ISDIR(st.st_mode);
  size_t size = is_dir ? 0 : st.st_size;
  
  ESP_LOGVV(TAG, "File info: %s, size: %d, is_dir: %s", 
            path.c_str(), size, is_dir ? "yes" : "no");
  
  return FileInfo(path, size, is_dir);
}

std::vector<FileInfo> SDStorage::direct_list_directory(const std::string &path) {
  std::vector<FileInfo> result;
  std::string full_path = this->get_full_path(path);
  
  DIR *dir = opendir(full_path.c_str());
  if (dir == nullptr) {
    ESP_LOGW(TAG, "Cannot open directory: %s", full_path.c_str());
    return result;
  }
  
  struct dirent *entry;
  while ((entry = readdir(dir)) != nullptr) {
    // Ignorer . et ..
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }
    
    std::string entry_path = path.empty() ? entry->d_name : path + "/" + entry->d_name;
    std::string full_entry_path = full_path + "/" + entry->d_name;
    
    struct stat st;
    if (stat(full_entry_path.c_str(), &st) == 0) {
      bool is_dir = S_ISDIR(st.st_mode);
      size_t size = is_dir ? 0 : st.st_size;
      result.emplace_back(entry_path, size, is_dir);
    }
  }
  
  closedir(dir);
  
  ESP_LOGVV(TAG, "Listed directory %s: %d entries", full_path.c_str(), result.size());
  return result;
}

uint8_t SDStorage::direct_read_byte(size_t offset) {
  if (this->current_file_ == nullptr) {
    ESP_LOGE(TAG, "No file set for reading");
    return 0;
  }
  
  if (fseek(this->current_file_, offset, SEEK_SET) != 0) {
    ESP_LOGE(TAG, "Failed to seek to offset %d", offset);
    return 0;
  }
  
  int byte = fgetc(this->current_file_);
  if (byte == EOF) {
    ESP_LOGVV(TAG, "EOF reached at offset %d", offset);
    return 0;
  }
  
  return static_cast<uint8_t>(byte);
}

bool SDStorage::direct_write_byte(uint8_t data) {
  if (this->current_file_ == nullptr) {
    ESP_LOGE(TAG, "No file set for writing");
    return false;
  }
  
  if (fputc(data, this->current_file_) == EOF) {
    ESP_LOGE(TAG, "Failed to write byte");
    return false;
  }
  
  fflush(this->current_file_);
  return true;
}

bool SDStorage::direct_append_byte(uint8_t data) {
  if (this->current_file_ == nullptr) {
    ESP_LOGE(TAG, "No file set for appending");
    return false;
  }
  
  // Aller à la fin du fichier
  if (fseek(this->current_file_, 0, SEEK_END) != 0) {
    ESP_LOGE(TAG, "Failed to seek to end of file");
    return false;
  }
  
  if (fputc(data, this->current_file_) == EOF) {
    ESP_LOGE(TAG, "Failed to append byte");
    return false;
  }
  
  fflush(this->current_file_);
  return true;
}

size_t SDStorage::direct_read_byte_array(size_t offset, uint8_t *data, size_t data_length) {
  if (this->current_file_ == nullptr) {
    ESP_LOGE(TAG, "No file set for reading");
    return 0;
  }
  
  if (fseek(this->current_file_, offset, SEEK_SET) != 0) {
    ESP_LOGE(TAG, "Failed to seek to offset %d", offset);
    return 0;
  }
  
  size_t bytes_read = fread(data, 1, data_length, this->current_file_);
  ESP_LOGVV(TAG, "Read %d bytes from offset %d", bytes_read, offset);
  
  return bytes_read;
}

bool SDStorage::direct_write_byte_array(uint8_t *data, size_t data_length) {
  if (this->current_file_ == nullptr) {
    ESP_LOGE(TAG, "No file set for writing");
    return false;
  }
  
  size_t bytes_written = fwrite(data, 1, data_length, this->current_file_);
  fflush(this->current_file_);
  
  if (bytes_written != data_length) {
    ESP_LOGE(TAG, "Failed to write all bytes (%d/%d)", bytes_written, data_length);
    return false;
  }
  
  ESP_LOGVV(TAG, "Wrote %d bytes", bytes_written);
  return true;
}

bool SDStorage::direct_append_byte_array(uint8_t *data, size_t data_length) {
  if (this->current_file_ == nullptr) {
    ESP_LOGE(TAG, "No file set for appending");
    return false;
  }
  
  // Aller à la fin du fichier
  if (fseek(this->current_file_, 0, SEEK_END) != 0) {
    ESP_LOGE(TAG, "Failed to seek to end of file");
    return false;
  }
  
  size_t bytes_written = fwrite(data, 1, data_length, this->current_file_);
  fflush(this->current_file_);
  
  if (bytes_written != data_length) {
    ESP_LOGE(TAG, "Failed to append all bytes (%d/%d)", bytes_written, data_length);
    return false;
  }
  
  ESP_LOGVV(TAG, "Appended %d bytes", bytes_written);
  return true;
}

}  // namespace storage
}  // namespace esphome
