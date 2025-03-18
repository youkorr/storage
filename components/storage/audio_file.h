#pragma once

#include <string>

namespace esphome {
namespace audio {

class AudioFile {
 public:
  virtual ~AudioFile() = default;
  virtual std::string get_file_path() const = 0;
  virtual bool is_valid() const = 0;
};

}  // namespace audio
}  // namespace esphome
