#include "box3web.h"
#include "esphome/core/log.h"
#include "esphome/components/network/util.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace box3web {

static const char *TAG = "box3web";

// Helper functions for startsWith and endsWith (for maximum compatibility)
bool endsWith(const std::string& str, const std::string& suffix) {
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool startsWith(const std::string& str, const std::string& prefix) {
  return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

Box3Web::Box3Web(web_server_base::WebServerBase *base) : base_(base) {}

void Box3Web::setup() { this->base_->add_handler(this); }

void Box3Web::dump_config() {
  ESP_LOGCONFIG(TAG, "Box3Web:");
  ESP_LOGCONFIG(TAG, "  Address: %s:%u", network::get_use_address().c_str(), this->base_->get_port());
  ESP_LOGCONFIG(TAG, "  Url Prefix: %s", this->url_prefix_.c_str());
  ESP_LOGCONFIG(TAG, "  Root Path: %s", this->root_path_.c_str());
  ESP_LOGCONFIG(TAG, "  Deletation Enabled: %s", TRUEFALSE(this->deletion_enabled_));
  ESP_LOGCONFIG(TAG, "  Download Enabled : %s", TRUEFALSE(this->download_enabled_));
  ESP_LOGCONFIG(TAG, "  Upload Enabled : %s", TRUEFALSE(this->upload_enabled_));
}

bool Box3Web::canHandle(AsyncWebServerRequest *request) {
  ESP_LOGD(TAG, "can handle %s %u", request->url().c_str(),
           str_startswith(std::string(request->url().c_str()), this->build_prefix()));
  return str_startswith(std::string(request->url().c_str()), this->build_prefix());
}

void Box3Web::handleRequest(AsyncWebServerRequest *request) {
  ESP_LOGD(TAG, "%s", request->url().c_str());
  if (str_startswith(std::string(request->url().c_str()), this->build_prefix())) {
    if (request->method() == HTTP_GET) {
      this->handle_get(request);
      return;
    }
    if (request->method() == HTTP_DELETE) {
      this->handle_delete(request);
      return;
    }
    if (request->method() == HTTP_POST && this->upload_enabled_) {
      // For POST methods, handle_upload will be called through handleUpload
      // Just acknowledge the request here if needed
      return;
    }

    // Handle unsupported methods
    request->send(405, "application/json", "{ \"error\": \"Method not allowed\" }");
  }
}

void Box3Web::handleUpload(AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data,
                            size_t len, bool final) {
  if (!this->upload_enabled_) {
    request->send(401, "application/json", "{ \"error\": \"file upload is disabled\" }");
    return;
  }
  std::string extracted = this->extract_path_from_url(std::string(request->url().c_str()));
  std::string path = this->build_absolute_path(extracted);

  if (index == 0 && !this->sd_mmc_card_->is_directory(path)) {
    auto response = request->beginResponse(401, "application/json", "{ \"error\": \"invalid upload folder\" }");
    response->addHeader("Connection", "close");
    request->send(response);
    return;
  }
  std::string file_name(filename.c_str());
  if (index == 0) {
    ESP_LOGD(TAG, "uploading file %s to %s", file_name.c_str(), path.c_str());
    this->sd_mmc_card_->write_file(Path::join(path, file_name).c_str(), data, len);
    return;
  }
  this->sd_mmc_card_->append_file(Path::join(path, file_name).c_str(), data, len);
  if (final) {
    auto response = request->beginResponse(201, "text/html", "upload success");
    response->addHeader("Connection", "close");
    request->send(response);
    return;
  }
}

void Box3Web::set_url_prefix(std::string const &prefix) { this->url_prefix_ = prefix; }

void Box3Web::set_root_path(std::string const &path) { this->root_path_ = path; }

void Box3Web::set_sd_mmc_card(sd_mmc_card::SdMmc *card) { this->sd_mmc_card_ = card; }

void Box3Web::set_deletion_enabled(bool allow) { this->deletion_enabled_ = allow; }

void Box3Web::set_download_enabled(bool allow) { this->download_enabled_ = allow; }

void Box3Web::set_upload_enabled(bool allow) { this->upload_enabled_ = allow; }

void Box3Web::handle_get(AsyncWebServerRequest *request) const {
  std::string extracted = this->extract_path_from_url(std::string(request->url().c_str()));
  std::string path = this->build_absolute_path(extracted);

  if (!this->sd_mmc_card_->is_directory(path)) {
    handle_download(request, path);
    return;
  }

  handle_index(request, path);
}

// Get the MIME type based on file extension
String Box3Web::get_content_type(const std::string &path) const {
  String file_path = String(path.c_str());
  if (endsWith(file_path, ".html")) return "text/html";
  else if (endsWith(file_path, ".css")) return "text/css";
  else if (endsWith(file_path, ".js")) return "application/javascript";
  else if (endsWith(file_path, ".json")) return "application/json";
  else if (endsWith(file_path, ".png")) return "image/png";
  else if (endsWith(file_path, ".jpg") || endsWith(file_path, ".jpeg")) return "image/jpeg";
  else if (endsWith(file_path, ".gif")) return "image/gif";
  else if (endsWith(file_path, ".svg")) return "image/svg+xml";
  else if (endsWith(file_path, ".ico")) return "image/x-icon";
  else if (endsWith(file_path, ".mp3")) return "audio/mpeg";
  else if (endsWith(file_path, ".wav")) return "audio/wav";
  else if (endsWith(file_path, ".mp4")) return "video/mp4";
  else if (endsWith(file_path, ".pdf")) return "application/pdf";
  else if (endsWith(file_path, ".zip")) return "application/zip";
  else if (endsWith(file_path, ".txt")) return "text/plain";
  else if (endsWith(file_path, ".xml")) return "application/xml";
  return "application/octet-stream";
}

void Box3Web::write_row(AsyncResponseStream *response, sd_mmc_card::FileInfo const &info) const {
  std::string uri = "/" + Path::join(this->url_prefix_, Path::remove_root_path(info.path, this->root_path_));
  std::string file_name = Path::file_name(info.path);
  std::string file_size = info.is_directory ? "-" : std::to_string(info.size);

  // Determine file type
  std::string file_type;
  if (info.is_directory) {
    file_type = "Directory";
  } else {
    String content_type = get_content_type(info.path);
    if (startsWith(content_type, "image/")) file_type = "Image";
    else if (startsWith(content_type, "audio/")) file_type = "Audio";
    else if (startsWith(content_type, "video/")) file_type = "Video";
    else if (startsWith(content_type, "text/")) file_type = "Text";
    else file_type = "File";
  }

  std::string row = "<tr>";
  row += "<td><a href='" + uri + "'>" + file_name + "</a></td>";
  row += "<td>" + file_type + "</td>";
  row += "<td>" + file_size + "</td>";

  if (this->deletion_enabled_ && !info.is_directory) {
    row += "<td><a href='" + uri + "?delete=true'>Delete</a></td>";
  } else {
    row += "<td></td>";
  }

  row += "</tr>";
  response->print(row.c_str());
}

void Box3Web::handle_index(AsyncWebServerRequest *request, const std::string &path) const {
  auto *response = request->beginResponseStream("text/html");
  response->print("<!DOCTYPE html><html><head><title>File Browser</title>");
  response->print("<style>");
  response->print("body { font-family: sans-serif; }");
  response->print("table { border-collapse: collapse; width: 100%; }");
  response->print("th, td { text-align: left; padding: 8px; border-bottom: 1px solid #ddd; }");
  response->print("tr:hover { background-color: #f5f5f5; }");
  response->print("th { background-color: #4CAF50; color: white; }");
  response->print("</style>");
  response->print("</head><body><h1>File Browser</h1><table>");
  response->print("<tr><th>Name</th><th>Type</th><th>Size</th>");
  if (this->deletion_enabled_) {
    response->print("<th>Actions</th>");
  }
  response->print("</tr>");

  this->sd_mmc_card_->iterate_files(path, [this, response](const sd_mmc_card::FileInfo &info) {
    this->write_row(response, info);
  });

  response->print("</table></body></html>");
  request->send(response);
}

void Box3Web::handle_delete(AsyncWebServerRequest *request) {
  if (!this->deletion_enabled_) {
    request->send(403, "application/json", "{ \"error\": \"Deletion is disabled\" }");
    return;
  }

  std::string extracted = this->extract_path_from_url(std::string(request->url().c_str()));
  std::string file_path = this->build_absolute_path(extracted);

  if (this->sd_mmc_card_->delete_file(file_path)) {
    request->send(200, "application/json", "{ \"message\": \"File deleted successfully\" }");
  } else {
    request->send(500, "application/json", "{ \"error\": \"Failed to delete file\" }");
  }
}

std::string Box3Web::build_prefix() const { return "/" + this->url_prefix_; }

std::string Box3Web::build_absolute_path(const std::string &relative) const {
  return Path::join(this->root_path_, relative);
}

std::string Box3Web::extract_path_from_url(const std::string &url) const {
  std::string prefix = this->build_prefix();
  if (url.size() <= prefix.size()) {
    return "";
  }
  return url.substr(prefix.size());
}

void Box3Web::handle_download(AsyncWebServerRequest *request, const std::string &path) const {
  if (!this->download_enabled_) {
    request->send(403, "application/json", "{ \"error\": \"Downloads are disabled\" }");
    return;
  }

  auto file = this->sd_mmc_card_->read_file(path);
  if (file.empty()) {
    request->send(404, "application/json", "{ \"error\": \"File not found\" }");
    return;
  }

  String content_type = get_content_type(path);

  auto *response = request->beginResponse_P(200, content_type.c_str(), file.data(), file.size());

  if (endsWith(path, ".gz")) {
    response->setContentEncoding("gzip");
  }

  // Setting disposition to inline for video/mp4 and images to enable streaming
  if (content_type == "video/mp4" || startsWith(content_type, "image/")) {
    response->header("Content-Disposition", "inline");
  } else {
    response->header("Content-Disposition", "attachment");
  }

  request->send(response);
}

}  // namespace box3web
}  // namespace esphome


