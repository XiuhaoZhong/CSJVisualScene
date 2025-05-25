#ifndef __CSJLOGGER_H__
#define __CSJLOGGER_H__

#include <cstdint>
#include <stdexcept>

#include <spdlog/spdlog.h>

class CSJLogger final {
public:
    CSJLogger();
    ~CSJLogger();

    enum class LogLevel : uint8_t {
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    template<typename... TARGS>
    void log(LogLevel level, TARGS&&... args) {
        switch (level) {
        case LogLevel::DEBUG:
            m_pLogger->debug(std::forward<TARGS>(args)...);
            break;
        case LogLevel::INFO:
            m_pLogger->info(std::forward<TARGS>(args)...);
            break;
        case LogLevel::WARN:
            m_pLogger->warn(std::forward<TARGS>(args)...);
            break;
        case LogLevel::ERROR:
            m_pLogger->error(std::forward<TARGS>(args)...);
            break;
        case LogLevel::FATAL:
            m_pLogger->critical(std::forward<TARGS>(args)...);
            // TODO: Add a callback to deal with fatal error!
        default:
            break;
        }
    }

    template<typename... TARGS>
    void fatalback(TARGS&&... args) {
        const std::string format_str = fmt::format(std::forward<TARGS>(args)...);
        throw std::runtime_error(format_str);
    }

private:
    std::shared_ptr<spdlog::logger> m_pLogger;

};

#endif // __CSJLOGGER_H__