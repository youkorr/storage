#pragma once

#include "esphome/components/sd_mmc_card/sd_mmc_card.h"
#include <ESPAsyncWebSrv.h>
#include <string>

namespace esphome {
namespace audio_http_bridge {

class AudioHttpBridge {
 public:
  explicit AudioHttpBridge(AsyncWebServer *server);

  void setup();
  void dump_config();

  // Configurateurs
  void set_url_prefix(std::string const &prefix);
  void set_audio_root_path(std::string const &path);
  void set_sd_mmc_card(sd_mmc_card::SdMmc *card);

 protected:
  // Helpers
  std::string build_prefix() const;
  std::string extract_file_path(std::string const &url) const;
  String get_content_type(std::string const &path) const;

  AsyncWebServer *server_{nullptr};
  sd_mmc_card::SdMmc *sd_mmc_card_{nullptr};
  std::string url_prefix_ = "audio";
  std::string audio_root_path_ = "/sdcard/audio";
};

}  // namespace audio_http_bridge
}  // namespace esphome
