#pragma once
#include "esphome/core/component.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include <vector>
#include <string>

namespace esphome {
namespace storage {

// Déclaration anticipée de StorageComponent
class StorageComponent;

class StorageFile : public Component {
 public:
  // Constructeur original qui prend un parent
  StorageFile(StorageComponent *parent) : parent_(parent) {}
  
  // Ajout d'un constructeur par défaut
  StorageFile() : parent_(nullptr) {}
  
  void set_path(const std::string &path) { path_ = path; }
  std::string get_path() const { return path_; }
  std::vector<uint8_t> read();
  
  // Ajout d'une méthode pour définir le parent après la construction
  void set_parent(StorageComponent *parent) { parent_ = parent; }
  
  // Getter pour parent_
  StorageComponent* get_parent() const { return parent_; }
  
 private:
  std::string path_;
  StorageComponent *parent_;
  
  // Pour que StorageComponent puisse accéder à parent_
  friend class StorageComponent;
};

class StorageComponent : public Component {
 public:
  void set_platform(const std::string &platform) { platform_ = platform; }
  void set_web_server(httpd_handle_t server) { server_ = server; }
  
  void add_file(StorageFile *file) {
    // Si le fichier n'a pas de parent, définir this comme parent
    if (file->get_parent() == nullptr) {
      file->set_parent(this);
    }
    files_.push_back(file);
  }
  
  void setup() override;
  void on_setup_web_server();
  std::string get_platform() const { return platform_; }
  
 protected:
  void setup_sd_card();
  void setup_flash();
  void setup_inline();
  esp_err_t serve_file(httpd_req_t *req);
  
 private:
  std::string platform_;
  httpd_handle_t server_{nullptr};
  std::vector<StorageFile*> files_;
};

}  // namespace storage

// Pour résoudre le problème de conversion entre StorageFile et AudioFile,
// si vous supprimez audio_file comme suggéré
namespace audio {
  // Définir un alias ou une classe vide pour AudioFile si nécessaire
  // Ceci permet d'avoir une compatibilité minimale
  class AudioFile {
  public:
    // Constructeur qui prend un StorageFile
    AudioFile(storage::StorageFile* storage_file) : storage_file_(storage_file) {}
    
    // Méthodes minimales nécessaires
    // À adapter selon vos besoins
    storage::StorageFile* get_storage_file() const { return storage_file_; }
    
  private:
    storage::StorageFile* storage_file_;
  };
  
  // Fonction helper pour convertir
  inline AudioFile* create_audio_file_from_storage(storage::StorageFile* storage_file) {
    return new AudioFile(storage_file);
  }
}

}  // namespace esphome












