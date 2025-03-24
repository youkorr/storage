#pragma once

#include <string>
#include "esphome/components/web_server_base/web_server_base.h"
#include "esphome/components/sd_mmc_card/sd_mmc_card.h"
#include "esphome/core/component.h"  // Ajout de Component

namespace esphome {
namespace box3web {

class Path {
 public:
  static const char separator = '/';
  static std::string file_name(std::string const &path);
  static bool is_absolute(std::string const &path);
  static bool trailing_slash(std::string const &path);
  static std::string join(std::string const &first, std::string const &second);
  static std::string remove_root_path(std::string path, std::string const &root);
};

class Box3Web : public Component, public AsyncWebHandler {  // Héritage de Component
 public:
  Box3Web(web_server_base::WebServerBase *base);

  void setup() override;  // Méthode obligatoire
  void dump_config() override;  // Méthode obligatoire

  void set_url_prefix(std::string const &prefix);
  void set_root_path(std::string const &path);
  void set_sd_mmc_card(sd_mmc_card::SdMmc *card);

  void set_deletion_enabled(bool allow);
  void set_download_enabled(bool allow);
  void set_upload_enabled(bool allow);

  bool canHandle(AsyncWebServerRequest *request) override;
  void handleRequest(AsyncWebServerRequest *request) override;
  void handleUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data,
                    size_t len, bool final) override;

 private:
  web_server_base::WebServerBase *base_{nullptr};
  sd_mmc_card::SdMmc *sd_mmc_card_{nullptr};

  std::string url_prefix_{"box3web"};
  std::string root_path_{"/sdcard"};

  bool deletion_enabled_{true};
  bool download_enabled_{true};
  bool upload_enabled_{true};

  void handle_get(AsyncWebServerRequest *request) const;
  void handle_index(AsyncWebServerRequest *request, std::string const &path) const;
  void handle_download(AsyncWebServerRequest *request, std::string const &path) const;
  void handle_delete(AsyncWebServerRequest *request);

  void write_row(AsyncResponseStream *response, sd_mmc_card::FileInfo const &info) const;

  String get_content_type(const std::string &path) const;
  std::string build_prefix() const;
  std::string extract_path_from_url(std::string const &url) const;
  std::string build_absolute_path(std::string relative_path) const;

  const char *component_source_{nullptr};  // Variable pour set_component_source (facultatif)
};

}  // namespace box3web
}  // namespace esphome
