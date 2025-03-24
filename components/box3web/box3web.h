#pragma once

#include <ESPAsyncWebServer.h>
#include "esphome/core/component.h"
#include "esphome/components/sd_mmc_card/sd_mmc_card.h"

namespace esphome {
namespace box3web {

class Box3Web : public Component {
 public:
  explicit Box3Web(AsyncWebServer *server);

  void setup() override;
  void dump_config() override;

 private:
  AsyncWebServer *server_;

  void on_request(AsyncWebServerRequest *request);
  void handle_get(AsyncWebServerRequest *request) const;
  void handle_index(AsyncWebServerRequest *request, const std::string &path) const;
  void handle_download(AsyncWebServerRequest *request, const std::string &path) const;
  void handle_delete(AsyncWebServerRequest *request);

  String get_content_type(const std::string &path) const;
  std::string build_prefix() const;
  std::string extract_path_from_url(const std::string &url) const;
  std::string build_absolute_path(const std::string &relative_path) const;

  sd_mmc_card::SdMmc *sd_mmc_card_{nullptr};
  std::string url_prefix_{"files"};
  std::string root_path_{"/sdcard"};
  bool deletion_enabled_{true};
  bool download_enabled_{true};
  bool upload_enabled_{true};
};

}  // namespace box3web
}  // namespace esphome
