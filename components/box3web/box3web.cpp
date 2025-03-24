#include "box3web.h"
#include "esphome/core/log.h"

namespace esphome {
namespace box3web {

static const char *TAG = "box3web";

Box3Web::Box3Web(AsyncWebServer *server) : server_(server) {}

void Box3Web::setup() {
  server_->addHandler(new AsyncCallbackWebHandler([this](AsyncWebServerRequest *request) {
    this->on_request(request);
  }));
}

void Box3Web::dump_config() {
  ESP_LOGCONFIG(TAG, "Box3Web:");
  ESP_LOGCONFIG(TAG, "  Url Prefix: %s", this->url_prefix_.c_str());
  ESP_LOGCONFIG(TAG, "  Root Path: %s", this->root_path_.c_str());
  ESP_LOGCONFIG(TAG, "  Deletion Enabled: %s", TRUEFALSE(this->deletion_enabled_));
  ESP_LOGCONFIG(TAG, "  Download Enabled: %s", TRUEFALSE(this->download_enabled_));
  ESP_LOGCONFIG(TAG, "  Upload Enabled: %s", TRUEFALSE(this->upload_enabled_));
}

void Box3Web::on_request(AsyncWebServerRequest *request) {
  if (str_startswith(request->url(), this->build_prefix())) {
    if (request->method() == HTTP_GET) {
      this->handle_get(request);
    } else if (request->method() == HTTP_DELETE) {
      this->handle_delete(request);
    } else if (request->method() == HTTP_POST && this->upload_enabled_) {
      request->send(200, "text/plain", "Upload acknowledged");
    } else {
      request->send(405, "application/json", "{ \"error\": \"Method not allowed\" }");
    }
  } else {
    request->send(404, "application/json", "{ \"error\": \"Not found\" }");
  }
}

void Box3Web::handle_get(AsyncWebServerRequest *request) const {
  std::string path = this->build_absolute_path(this->extract_path_from_url(request->url()));
  if (!this->sd_mmc_card_->is_directory(path)) {
    this->handle_download(request, path);
    return;
  }
  this->handle_index(request, path);
}

String Box3Web::get_content_type(const std::string &path) const {
  String file_path = String(path.c_str());
  if (file_path.endsWith(".html")) return "text/html";
  else if (file_path.endsWith(".css")) return "text/css";
  else if (file_path.endsWith(".js")) return "application/javascript";
  else if (file_path.endsWith(".json")) return "application/json";
  else if (file_path.endsWith(".png")) return "image/png";
  else if (file_path.endsWith(".jpg") || file_path.endsWith(".jpeg")) return "image/jpeg";
  else if (file_path.endsWith(".gif")) return "image/gif";
  else if (file_path.endsWith(".svg")) return "image/svg+xml";
  else if (file_path.endsWith(".ico")) return "image/x-icon";
  else if (file_path.endsWith(".mp3")) return "audio/mpeg";
  else if (file_path.endsWith(".wav")) return "audio/wav";
  else if (file_path.endsWith(".mp4")) return "video/mp4";
  else if (file_path.endsWith(".pdf")) return "application/pdf";
  else if (file_path.endsWith(".zip")) return "application/zip";
  else if (file_path.endsWith(".txt")) return "text/plain";
  else if (file_path.endsWith(".xml")) return "application/xml";
  return "application/octet-stream";
}

std::string Box3Web::build_prefix() const {
  if (this->url_prefix_.length() == 0 || this->url_prefix_[0] != '/') return "/" + this->url_prefix_;
  return this->url_prefix_;
}

std::string Box3Web::extract_path_from_url(const std::string &url) const {
  return url.substr(this->build_prefix().size());
}

std::string Box3Web::build_absolute_path(const std::string &relative_path) const {
  return Path::join(this->root_path_, relative_path);
}

}  // namespace box3web
}  // namespace esphome



