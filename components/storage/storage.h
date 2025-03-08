#pragma once
#include "esphome/core/component.h"
#include "esphome/components/web_server_base/web_server_base.h"
#include <ESPAsyncWebServer.h>
#include <vector>
#include <string>

namespace esphome {
namespace storage {

class StorageFile : public Component {
 public:
  void set_path(const std::string &path) { path_ = path; }
  std::string get_path() const { return path_; }
  void set_platform(const std::string &platform) { platform_ = platform; }
  std::string get_platform() const { return platform_; }
  std::vector<uint8_t> read();
  
 private:
  std::string path_;
  std::string platform_;
};

class StorageComponent : public Component {
 public:
  void set_platform(const std::string &platform) { platform_ = platform; }
  void set_web_server(web_server_base::WebServerBase *web_server) { web_server_ = web_server; }
  
  void add_file(StorageFile *file) {
    files_.push_back(file);
  }
  
  void setup() override;
  void on_setup_web_server();
  
 protected:
  void setup_sd_card();
  void setup_flash();
  void setup_inline();
  void serve_file(StorageFile *file, AsyncWebServerRequest *request);
  
 private:
  std::string platform_;
  web_server_base::WebServerBase *web_server_{nullptr};
  std::vector<StorageFile*> files_;
};

// Définition anticipée pour l'utilisation dans storage.cpp
class AsyncCallbackWebHandler : public AsyncWebHandler {
public:
  using RequestCallback = std::function<void(AsyncWebServerRequest*)>;
  
  AsyncCallbackWebHandler(const char* uri, RequestCallback callback);
  
  bool canHandle(AsyncWebServerRequest *request) override;
  void handleRequest(AsyncWebServerRequest *request) override;

private:
  std::string uri_;
  RequestCallback callback_;
};

}  // namespace storage
}  // namespace esphome












