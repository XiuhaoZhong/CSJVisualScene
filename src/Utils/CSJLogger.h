#ifndef __CSJLOGGER_H__
#define __CSJLOGGER_H__

#include <cstdint>
#include <stdexcept>

#include <spdlog/spdlog.h>

class CSJLogger final {
public:
    CSJLogger();
    ~CSJLogger();

    static CSJLogger* getLoggerInst();

    enum class LogLevel : uint8_t {
        DEBUG_LOG,
        INFO_LOG,
        WARN_LOG,
        ERROR_LOG,
        FATAL_LOG
    };

    template<typename... TARGS>
    void log_debug(TARGS&&... args) {
        log(LogLevel::DEBUG_LOG, args...);
    }

    template<typename... TARGS>
    void log_info(TARGS&&... args) {
        log(LogLevel::INFO_LOG, args...);
    }

    template<typename... TARGS>
    void log_warn(TARGS&&... args) {
        log(LogLevel::WARN_LOG, args...);
    }

    template<typename... TARGS>
    void log_error(TARGS&&... args) {
        log(LogLevel::ERROR_LOG, args...);
    }

    template<typename... TARGS>
    void log_fatal(TARGS&&... args) {
        log(LogLevel::FATAL_LOG, args...);
    }

    template<typename... TARGS>
    void log(LogLevel level, TARGS&&... args) {
        switch (level) {
        case LogLevel::DEBUG_LOG:
            m_pLogger->debug(std::forward<TARGS>(args)...);
            break;
        case LogLevel::INFO_LOG:
            m_pLogger->info(std::forward<TARGS>(args)...);
            break;
        case LogLevel::WARN_LOG:
            m_pLogger->warn(std::forward<TARGS>(args)...);
            break;
        case LogLevel::ERROR_LOG:
            m_pLogger->error(std::forward<TARGS>(args)...);
            break;
        case LogLevel::FATAL_LOG:
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