#include "storage.h"
#include "esphome/core/log.h"
#include <fstream>

namespace esphome {
namespace storage {

static const char *const TAG = "storage";

std::vector<uint8_t> StorageFile::read() {
  std::vector<uint8_t> data;
  
  if (platform_ == "sd_card") {
    std::ifstream file(path_, std::ios::binary);
    if (file) {
      file.seekg(0, std::ios::end);
      data.resize(file.tellg());
      file.seekg(0, std::ios::beg);
      file.read(reinterpret_cast<char*>(data.data()), data.size());
    }
  } else if (platform_ == "flash") {
    // Implémentation pour la mémoire flash
  } else if (platform_ == "inline") {
    // Implémentation pour le stockage inline
  }

  if (data.empty()) {
    ESP_LOGE(TAG, "Échec de la lecture du fichier : %s", path_.c_str());
  }

  return data;
}

void StorageComponent::setup() {
  ESP_LOGD(TAG, "Configuration du stockage : %s", platform_.c_str());
  
  if (platform_ == "sd_card") {
    setup_sd_card();
  } else if (platform_ == "flash") {
    setup_flash();
  } else if (platform_ == "inline") {
    setup_inline();
  }

  if (web_server_ != nullptr) {
    this->on_setup_web_server();
  }
}

void StorageComponent::on_setup_web_server() {
  for (auto *file : files_) {
    std::string url = "/media/" + file->get_path();
    web_server_->on(url.c_str(), HTTP_GET, [this, file](AsyncWebServerRequest *req) {
      this->serve_file(file, req);
    });
    ESP_LOGD(TAG, "URL enregistrée : %s", url.c_str());
  }
}

void StorageComponent::serve_file(StorageFile *file, AsyncWebServerRequest *req) {
  auto data = file->read();
  if (!data.empty()) {
    req->send(200, "audio/mpeg", data.data(), data.size());
  } else {
    req->send(404, "text/plain", "Fichier non trouvé");
  }
}

void StorageComponent::setup_sd_card() {
  ESP_LOGD(TAG, "Initialisation de la carte SD");
  // Ajoutez ici l'initialisation de la SD Card (ex: SD.begin())
}

void StorageComponent::setup_flash() {
  ESP_LOGD(TAG, "Initialisation de la mémoire flash");
  // Ajoutez ici l'initialisation de la mémoire flash (ex: SPIFFS)
}

void StorageComponent::setup_inline() {
  ESP_LOGD(TAG, "Initialisation du stockage inline");
  // Implémentation spécifique au stockage inline
}

}  // namespace storage
}  // namespace esphome



