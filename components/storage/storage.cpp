#include "storage.h"
#include "esphome/core/log.h"
#include "esphome/components/web_server_base/web_server_base.h"
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
    // Implementation for flash storage
  } else if (platform_ == "inline") {
    // Implementation for inline storage
  }
  if (data.empty()) {
    ESP_LOGE(TAG, "Failed to read file: %s", path_.c_str());
  }
  return data;
}

void StorageComponent::setup() {
  ESP_LOGD(TAG, "Setting up storage platform: %s", platform_.c_str());
  
  if (platform_ == "sd_card") {
    setup_sd_card();
  } else if (platform_ == "flash") {
    setup_flash();
  } else if (platform_ == "inline") {
    setup_inline();
  }

  // Propagate platform to all files
  for (auto *file : files_) {
    file->set_platform(platform_);
  }
  
  if (web_server_ != nullptr) {
    this->on_setup_web_server();
  }
}

void StorageComponent::on_setup_web_server() {
  for (auto *file : files_) {
    std::string url = "/media/" + file->get_path();
    
    // Use the add_handler method with proper AsyncWebHandler implementation
    web_server_->add_handler(new AsyncCallbackWebHandler(
      url.c_str(), 
      [this, file](AsyncWebServerRequest *request) {
        this->serve_file(file, request);
      }
    ));
    
    ESP_LOGD(TAG, "Registered media URL: %s", url.c_str());
  }
}

void StorageComponent::serve_file(StorageFile *file, AsyncWebServerRequest *request) {
  auto data = file->read();
  if (!data.empty()) {
    // Determine content type based on file extension
    std::string content_type = "application/octet-stream";
    std::string path = file->get_path();
    
    size_t ext_pos = path.find_last_of('.');
    if (ext_pos != std::string::npos) {
      std::string ext = path.substr(ext_pos + 1);
      if (ext == "mp3") {
        content_type = "audio/mpeg";
      } else if (ext == "wav") {
        content_type = "audio/wav";
      } else if (ext == "txt") {
        content_type = "text/plain";
      } else if (ext == "html" || ext == "htm") {
        content_type = "text/html";
      } else if (ext == "css") {
        content_type = "text/css";
      } else if (ext == "js") {
        content_type = "application/javascript";
      } else if (ext == "json") {
        content_type = "application/json";
      } else if (ext == "png") {
        content_type = "image/png";
      } else if (ext == "jpg" || ext == "jpeg") {
        content_type = "image/jpeg";
      }
    }
    
    // Send the file with appropriate content type
    AsyncWebServerResponse *response = request->beginResponse_P(
      200, content_type.c_str(), reinterpret_cast<const uint8_t*>(data.data()), data.size()
    );
    request->send(response);
  } else {
    request->send(404, "text/plain", "File not found");
  }
}

void StorageComponent::setup_sd_card() {
  ESP_LOGD(TAG, "Initializing SD card storage");
}

void StorageComponent::setup_flash() {
  ESP_LOGD(TAG, "Initializing flash storage");
}

void StorageComponent::setup_inline() {
  ESP_LOGD(TAG, "Initializing inline storage");
}

// Class definition for AsyncCallbackWebHandler to handle web requests
class AsyncCallbackWebHandler : public AsyncWebHandler {
public:
  using RequestCallback = std::function<void(AsyncWebServerRequest*)>;
  
  AsyncCallbackWebHandler(const char* uri, RequestCallback callback)
    : uri_(uri), callback_(callback) {}
  
  bool canHandle(AsyncWebServerRequest *request) override {
    return request->url() == uri_ && request->method() == HTTP_GET;
  }
  
  void handleRequest(AsyncWebServerRequest *request) override {
    callback_(request);
  }

private:
  std::string uri_;
  RequestCallback callback_;
};

}  // namespace storage
}  // namespace esphome



