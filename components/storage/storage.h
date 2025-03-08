#pragma once

#include "esphome/core/component.h"
#include "esphome/components/web_server/web_server.h"
#include <vector>
#include <string>
#include <functional>

namespace esphome {
namespace storage {

class StorageComponent : public Component {
 public:
  void set_platform(const std::string &platform) { platform_ = platform; }
  void set_web_server(web_server_base::WebServerBase *web_server) { web_server_ = web_server; }
  
  void add_file(std::function<std::vector<uint8_t>()> source, const std::string &id) {
    files_.emplace_back(source, id);
  }
  
  std::string get_file_url(const std::string &id) const {
    for (const auto &file : files_) {
      if (file.second == id) {
        return "/media/" + file.second;
      }
    }
    return "";
  }

  std::vector<uint8_t> read_file(char const *path);
  std::vector<uint8_t> read_file(std::string const &path);

  void setup() override;
  void on_setup_web_server();

 protected:
  void setup_sd_card();
  void setup_flash();
  void setup_inline();
  void serve_file(const std::string &id);

 private:
  std::string platform_;
  web_server_base::WebServerBase *web_server_{nullptr};
  std::vector<std::pair<std::function<std::vector<uint8_t>()>, std::string>> files_;
};

}  // namespace storage
}  // namespace esphome












