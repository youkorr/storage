#pragma once

#include "esphome/components/sd_mmc_card/sd_mmc_card.h"
#include "esphome/components/web_server_base/web_server_base.h"
#include <string>
#include <vector>
#include <ESPAsyncWebServer.h>

namespace esphome {
namespace box3web {

class Path {
 public:
  static constexpr char separator = '/';

  static std::string file_name(std::string const &path);
  static bool is_absolute(std::string const &path);
  static bool trailing_slash(std::string const &path);
  static std::string join(std::string const &first, std::string const &second);
  static std::string remove_root_path(std::string path, std::string const &root);
};

class Box3Web : public AsyncWebHandler {
 public:
  explicit Box3Web(AsyncWebServer *server);

  void setup();
  void dump_config();

  // Configurateurs
  void set_url_prefix(std::string const &prefix);
  void set_root_path(std::string const &path);
  void set_sd_mmc_card(sd_mmc_card::SdMmc *card);
  void set_deletion_enabled(bool allow);
  void set_download_enabled(bool allow);
  void set_upload_enabled(bool allow);

  // Implémentation de AsyncWebHandler
  bool canHandle(AsyncWebServerRequest *request) override {
    return str_startswith(std::string(request->url().c_str()), this->build_prefix());
  }

  void handleRequest(AsyncWebServerRequest *request) override {
    // Cette méthode ne sera pas utilisée car nous utilisons le système de route d'ESPAsyncWebServer
  }

  void handleUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data,
                    size_t len, bool final);

 protected:
  void handle_get(AsyncWebServerRequest *request) const;
  void handle_delete(AsyncWebServerRequest *request);
  void handle_index(AsyncWebServerRequest *request, std::string const &path) const;
  void handle_download(AsyncWebServerRequest *request, std::string const &path) const;
  void write_row(AsyncResponseStream *response, sd_mmc_card::FileInfo const &info) const;

  // Helpers
  std::string build_prefix() const;
  std::string extract_path_from_url(std::string const &url) const;
  std::string build_absolute_path(std::string relative_path) const;

  // ESPAsyncWebServer au lieu de web_server_base
  AsyncWebServer *server_{nullptr};
  
  sd_mmc_card::SdMmc *sd_mmc_card_{nullptr};
  std::string url_prefix_ = "sdcard";
  std::string root_path_ = "/sdcard";
  bool deletion_enabled_ = false;
  bool download_enabled_ = true;
  bool upload_enabled_ = false;
};

}  // namespace box3web
}  // namespace esphome
