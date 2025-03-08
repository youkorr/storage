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
  
  // Propagation de platform_ vers les fichiers
  for (auto *file : files_) {
    file->set_platform(platform_);  // Ligne cruciale ajoutée
  }

  if (platform_ == "sd_card") {
    setup_sd_card();
  } else if (platform_ == "flash") {
    setup_flash();
  } else if (platform_ == "inline") {
    setup_inline();
  }

  // Configuration du serveur web
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.uri_match_fn = httpd_uri_match_wildcard;
  ESP_ERROR_CHECK(httpd_start(&server_, &config));
  this->on_setup_web_server();
}

void StorageComponent::on_setup_web_server() {
  for (auto *file : files_) {
    std::string url = "/media/" + file->get_path();
    httpd_uri_t uri = {
        .uri      = url.c_str(),
        .method   = HTTP_GET,
        .handler  = serve_file_handler,
        .user_ctx = this  // Passage du contexte (this)
    };
    httpd_register_uri_handler(server_, &uri);
    ESP_LOGD(TAG, "URL enregistrée : %s", url.c_str());
  }
}

// Handler de requête HTTP natif
esp_err_t StorageComponent::serve_file_handler(httpd_req_t *req) {
  StorageComponent *self = static_cast<StorageComponent*>(req->user_ctx);
  
  // Récupère le fichier correspondant à l'URL
  std::string path(req->uri);
  path.erase(0, 6);  // Supprime "/media/" du chemin
  for (auto *file : self->files_) {
    if (file->get_path() == path) {
      auto data = file->read();
      if (!data.empty()) {
        httpd_resp_set_type(req, "audio/mpeg");
        httpd_resp_send(req, reinterpret_cast<const char*>(data.data()), data.size());
        return ESP_OK;
      }
    }
  }
  
  httpd_resp_send_404(req);
  return ESP_OK;
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



