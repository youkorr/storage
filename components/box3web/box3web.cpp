#include "box3web.h"
#include "esphome/core/log.h"
#include "esphome/components/network/util.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace box3web {

static const char *TAG = "box3web";

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
    if (content_type.startsWith("image/")) file_type = "Image";
    else if (content_type.startsWith("audio/")) file_type = "Audio";
    else if (content_type.startsWith("video/")) file_type = "Video";
    else if (content_type.startsWith("text/")) file_type = "Text";
    else file_type = "File";
  }

  response->print("<tr><td>");
  if (info.is_directory) {
    response->print("<a href=\"");
    response->print(uri.c_str());
    response->print("\">");
    response->print(file_name.c_str());
    response->print("/</a>");
  } else {
    response->print("<a href=\"");
    response->print(uri.c_str());
    response->print("\">");
    response->print(file_name.c_str());
    response->print("</a>");
  }
  response->print("</td><td>");
  response->print(file_type.c_str());
  response->print("</td><td>");
  response->print(file_size.c_str());
  response->print("</td><td>");
  
  if (!info.is_directory) {
    if (this->download_enabled_) {
      response->print("<button onClick=\"download_file('");
      response->print(uri.c_str());
      response->print("','");
      response->print(file_name.c_str());
      response->print("')\">Download</button>");
    }
    if (this->deletion_enabled_) {
      response->print("<button onClick=\"delete_file('");
      response->print(uri.c_str());
      response->print("')\">Delete</button>");
    }
  }
  response->print("</td></tr>");
}

void Box3Web::handle_index(AsyncWebServerRequest *request, std::string const &path) const {
  AsyncResponseStream *response = request->beginResponseStream("text/html");
  response->print(F("<!DOCTYPE html><html lang=\"en\"><head><meta charset=UTF-8><meta "
                    "name=viewport content=\"width=device-width, initial-scale=1,user-scalable=no\">"
                    "<style>"
                    "body { font-family: Arial, sans-serif; margin: 0; padding: 20px; }"
                    "h1, h2 { color: #333; }"
                    "table { width: 100%; border-collapse: collapse; margin-top: 20px; }"
                    "th, td { padding: 8px; text-align: left; border-bottom: 1px solid #ddd; }"
                    "th { background-color: #f2f2f2; }"
                    "tr:hover { background-color: #f5f5f5; }"
                    "button { margin: 2px; padding: 5px 10px; background-color: #4CAF50; color: white; "
                    "border: none; border-radius: 4px; cursor: pointer; }"
                    "button:hover { background-color: #45a049; }"
                    ".delete { background-color: #f44336; }"
                    ".delete:hover { background-color: #d32f2f; }"
                    "a { color: #2196F3; text-decoration: none; }"
                    "a:hover { text-decoration: underline; }"
                    ".upload-form { margin: 20px 0; padding: 15px; background-color: #f9f9f9; border-radius: 4px; }"
                    "</style>"
                    "</head><body>"
                    "<h1>SD Card Content</h1><h2>Folder "));

  response->print(path.c_str());
  response->print(F("</h2>"));
  
  // Add breadcrumb navigation
  std::string current_path = Path::remove_root_path(path, this->root_path_);
  if (current_path != "/") {
    response->print("<div class=\"breadcrumb\"><a href=\"/");
    response->print(this->url_prefix_.c_str());
    response->print("\">Home</a> / ");
    
    std::vector<std::string> parts;
    std::string part;
    for (char c : current_path) {
      if (c == '/') {
        if (!part.empty()) {
          parts.push_back(part);
          part.clear();
        }
      } else {
        part += c;
      }
    }
    if (!part.empty()) {
      parts.push_back(part);
    }
    
    std::string cumulative_path = "";
    for (size_t i = 0; i < parts.size(); i++) {
      cumulative_path += "/" + parts[i];
      response->print("<a href=\"/");
      response->print(this->url_prefix_.c_str());
      response->print(cumulative_path.c_str());
      response->print("\">");
      response->print(parts[i].c_str());
      response->print("</a>");
      
      if (i < parts.size() - 1) {
        response->print(" / ");
      }
    }
    response->print("</div><br>");
  }
  
  if (this->upload_enabled_) {
    response->print(F("<div class=\"upload-form\">"
                      "<form method=\"POST\" enctype=\"multipart/form-data\">"
                      "<input type=\"file\" name=\"file\" multiple>"
                      "<input type=\"submit\" value=\"Upload File(s)\">"
                      "</form></div>"));
  }
  
  response->print(F("<table id=\"files\">"
                    "<thead><tr>"
                    "<th>Name</th>"
                    "<th>Type</th>"
                    "<th>Size</th>"
                    "<th>Actions</th>"
                    "</tr></thead><tbody>"));
  
  auto entries = this->sd_mmc_card_->list_directory_file_info(path, 0);
  for (auto const &entry : entries)
    write_row(response, entry);

  response->print(F("</tbody></table>"
                    "<script>"
                    "function delete_file(path) {"
                    "  if(confirm('Are you sure you want to delete this file?')) {"
                    "    fetch(path, {method: 'DELETE'})"
                    "    .then(response => {"
                    "      if(response.ok) {"
                    "        alert('File deleted successfully');"
                    "        location.reload();"
                    "      } else {"
                    "        alert('Error deleting file');"
                    "      }"
                    "    }).catch(error => {"
                    "      alert('Error: ' + error);"
                    "    });"
                    "  }"
                    "}"
                    "function download_file(path, filename) {"
                    "  fetch(path).then(response => response.blob())"
                    "  .then(blob => {"
                    "    const link = document.createElement('a');"
                    "    link.href = URL.createObjectURL(blob);"
                    "    link.download = filename;"
                    "    link.click();"
                    "  }).catch(error => {"
                    "    alert('Error downloading file: ' + error);"
                    "  });"
                    "}"
                    "</script>"
                    "</body></html>"));

  request->send(response);
}

void Box3Web::handle_download(AsyncWebServerRequest *request, std::string const &path) const {
  if (!this->download_enabled_) {
    request->send(401, "application/json", "{ \"error\": \"file download is disabled\" }");
    return;
  }

  auto file = this->sd_mmc_card_->read_file(path);
  if (file.size() == 0) {
    request->send(404, "application/json", "{ \"error\": \"failed to read file or file is empty\" }");
    return;
  }

  // Get the content type based on file extension
  String content_type = get_content_type(path);
  
#ifdef USE_ESP_IDF
  // Use the pointer-based response for ESP-IDF
  auto *response = request->beginResponse_P(200, content_type, file.data(), file.size());
  
  // For some content types, you might want to set additional headers
  if (content_type == "audio/mpeg" || content_type == "audio/wav" || 
      content_type == "video/mp4" || content_type.startsWith("image/")) {
    response->addHeader("Accept-Ranges", "bytes");
  }
  
  // Add filename for download
  std::string filename = Path::file_name(path);
  response->addHeader("Content-Disposition", "inline; filename=\"" + String(filename.c_str()) + "\"");
#else
  // For non-ESP-IDF, use stream-based response
  auto *response = request->beginResponseStream(content_type, file.size());
  
  // For some content types, you might want to set additional headers
  if (content_type == "audio/mpeg" || content_type == "audio/wav" || 
      content_type == "video/mp4" || content_type.startsWith("image/")) {
    response->addHeader("Accept-Ranges", "bytes");
  }
  
  // Add filename for download
  std::string filename = Path::file_name(path);
  response->addHeader("Content-Disposition", "inline; filename=\"" + String(filename.c_str()) + "\"");
  
  response->write(file.data(), file.size());
#endif

  request->send(response);
}

void Box3Web::handle_delete(AsyncWebServerRequest *request) {
  if (!this->deletion_enabled_) {
    request->send(401, "application/json", "{ \"error\": \"file deletion is disabled\" }");
    return;
  }
  std::string extracted = this->extract_path_from_url(std::string(request->url().c_str()));
  std::string path = this->build_absolute_path(extracted);
  if (this->sd_mmc_card_->is_directory(path)) {
    request->send(401, "application/json", "{ \"error\": \"cannot delete a directory\" }");
    return;
  }
  if (this->sd_mmc_card_->delete_file(path)) {
    request->send(204, "application/json", "{}");
    return;
  }
  request->send(401, "application/json", "{ \"error\": \"failed to delete file\" }");
}

std::string Box3Web::build_prefix() const {
  if (this->url_prefix_.length() == 0 || this->url_prefix_.at(0) != '/')
    return "/" + this->url_prefix_;
  return this->url_prefix_;
}

std::string Box3Web::extract_path_from_url(std::string const &url) const {
  std::string prefix = this->build_prefix();
  return url.substr(prefix.size(), url.size() - prefix.size());
}

std::string Box3Web::build_absolute_path(std::string relative_path) const {
  if (relative_path.size() == 0)
    return this->root_path_;

  std::string absolute = Path::join(this->root_path_, relative_path);
  return absolute;
}

std::string Path::file_name(std::string const &path) {
  size_t pos = path.rfind(Path::separator);
  if (pos != std::string::npos) {
    return path.substr(pos + 1);
  }
  return "";
}

bool Path::is_absolute(std::string const &path) { return path.size() && path[0] == separator; }

bool Path::trailing_slash(std::string const &path) { return path.size() && path[path.length() - 1] == separator; }

std::string Path::join(std::string const &first, std::string const &second) {
  std::string result = first;
  if (!trailing_slash(first) && !is_absolute(second)) {
    result.push_back(separator);
  }
  if (trailing_slash(first) && is_absolute(second)) {
    result.pop_back();
  }
  result.append(second);
  return result;
}

std::string Path::remove_root_path(std::string path, std::string const &root) {
  if (!str_startswith(path, root))
    return path;
  if (path.size() == root.size() || path.size() < 2)
    return "/";
  return path.erase(0, root.size());
}

}  // namespace box3web
}  // namespace esphome
