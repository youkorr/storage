#include "audio_http_bridge.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace audio_http_bridge {

static const char *TAG = "audio_http_bridge";

AudioHttpBridge::AudioHttpBridge(AsyncWebServer *server) : server_(server) {}

void AudioHttpBridge::setup() {
  // Configuration de la route pour servir les fichiers audio
  std::string path = this->build_prefix() + "/*";
  
  this->server_->on(path.c_str(), HTTP_GET, [this](AsyncWebServerRequest *request) {
    std::string url = request->url().c_str();
    std::string file_path = this->extract_file_path(url);
    
    ESP_LOGD(TAG, "Serving audio file: %s", file_path.c_str());
    
    if (!this->sd_mmc_card_->exists(file_path)) {
      request->send(404, "text/plain", "File not found");
      return;
    }
    
    // Déterminer le MIME type basé sur l'extension
    String content_type = this->get_content_type(file_path);
    
    auto file = this->sd_mmc_card_->read_file(file_path);
    if (file.size() == 0) {
      request->send(500, "text/plain", "Failed to read file");
      return;
    }
    
    AsyncWebServerResponse *response;
    
#ifdef USE_ESP_IDF
    // En mode ESP-IDF, utilisez la méthode optimisée pour les grands fichiers
    response = request->beginResponse_P(200, content_type.c_str(), file.data(), file.size());
#else
    // Fallback pour ESP8266/ESP32 en mode Arduino
    response = request->beginResponse_P(200, content_type.c_str(), file.data(), file.size());
#endif
    
    // Ajout d'en-têtes pour améliorer la diffusion
    response->addHeader("Accept-Ranges", "bytes");
    response->addHeader("Cache-Control", "max-age=3600");
    
    request->send(response);
  });
}

void AudioHttpBridge::dump_config() {
  ESP_LOGCONFIG(TAG, "Audio HTTP Bridge:");
  ESP_LOGCONFIG(TAG, "  URL Prefix: %s", this->url_prefix_.c_str());
  ESP_LOGCONFIG(TAG, "  Root Path: %s", this->audio_root_path_.c_str());
}

void AudioHttpBridge::set_url_prefix(std::string const &prefix) { 
  this->url_prefix_ = prefix;
}

void AudioHttpBridge::set_audio_root_path(std::string const &path) { 
  this->audio_root_path_ = path;
}

void AudioHttpBridge::set_sd_mmc_card(sd_mmc_card::SdMmc *card) { 
  this->sd_mmc_card_ = card;
}

std::string AudioHttpBridge::build_prefix() const {
  if (this->url_prefix_.length() == 0 || this->url_prefix_.at(0) != '/')
    return "/" + this->url_prefix_;
  return this->url_prefix_;
}

std::string AudioHttpBridge::extract_file_path(std::string const &url) const {
  std::string prefix = this->build_prefix();
  std::string relative_path = url.substr(prefix.size());
  
  // Si le chemin commence par un slash, on le retire
  if (relative_path.length() > 0 && relative_path[0] == '/')
    relative_path = relative_path.substr(1);
    
  return this->audio_root_path_ + "/" + relative_path;
}

String AudioHttpBridge::get_content_type(std::string const &path) const {
  if (path.ends_with(".mp3")) return "audio/mp3";
  if (path.ends_with(".wav")) return "audio/wav";
  if (path.ends_with(".ogg")) return "audio/ogg";
  if (path.ends_with(".flac")) return "audio/flac";
  if (path.ends_with(".aac")) return "audio/aac";
  if (path.ends_with(".m4a")) return "audio/mp4";
  
  // Type par défaut
  return "application/octet-stream";
}

}  // namespace audio_http_bridge
}  // namespace esphome
