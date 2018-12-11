#ifndef hcana_Logger_hh
#define hcana_Logger_hh

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h" //support for stdout logging
#include "spdlog/sinks/basic_file_sink.h" // support for basic file logging

namespace hcana {

  template <typename Base>
  class ConfigLogging : public Base {
  protected:
    std::shared_ptr<spdlog::logger> _logger;
  public:
    template <class... Args>
    ConfigLogging(Args&&... args) : Base(std::forward<Args>(args)...) {
      _logger = spdlog::get("config");
      if(!_logger) {
        _logger = spdlog::stdout_color_mt("config");
      }
      _logger->set_pattern("[%t] [%n] %^[%l]%$ %v");
    }

  };

  template <typename Base>
  class HitLogging : public Base {
  protected:
    std::shared_ptr<spdlog::logger> _hit_logger;
  public:
    template <class... Args>
    HitLogging(Args&&... args) : Base(std::forward<Args>(args)...) {
      _hit_logger = spdlog::get("hits");
      if(!_hit_logger) {
        _hit_logger = spdlog::stdout_color_mt("hits");
      }
    }

  };
}


#endif
