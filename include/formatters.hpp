#include <format>
#include <string>
#include <constants.hpp>
#include <wave.hpp>

#ifndef FORMATTER_SPECIALIZERS
#define FORMATTER_SPECIALIZERS

template<>
struct std::formatter<wave::FormatCode>: std::formatter<std::string>{

  auto format(wave::FormatCode var, format_context& ctx) const{
    std::string name; 
    switch (var) {
      case wave::WAVE_FORMAT_PCM:
        name = "WAVE_FORMAT_PCM";
        break;
      case wave::WAVE_FORMAT_IEEE_FLOAT:
        name = "WAVE_FORMAT_IEEE_FLOAT";
        break;
      case wave::WAVE_FORMAT_ALAW:
        name = "WAVE_FORMAT_ALAW";
        break;
      case wave::WAVE_FORMAT_MULAW:
        name = "WAVE_FORMAT_MULAW";
        break;
      case wave::WAVE_FORMAT_EXTENSIBLE:
        name = "WAVE_FORMAT_EXTENSIBLE";
        break;
      default:
        throw std::runtime_error("Undefined wave format");
    }
    
    return std::formatter<std::string>::format(name, ctx);
  }
};

#endif
