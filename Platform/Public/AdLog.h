#ifndef ADLOG_H
#define ADLOG_H

#include "AdEngine.h"

#include "spdlog/common.h"

namespace ade{
    class AdLog{
    public:
        AdLog() = default;
        AdLog(const AdLog&) = delete;
        AdLog &operator=(const AdLog&) = delete;
        static void Init();

        static AdLog* GetLoggerInstance(){
            return &sLoggerInstance;
        }

        template<typename... Args>
        void Log(spdlog::source_loc loc, spdlog::level::level_enum lvl, spdlog::format_string_t<Args...> fmt, Args &&...args){
            spdlog::memory_buf_t buf;
            fmt::vformat_to(fmt::appender(buf), fmt, fmt::make_format_args(args...));
            Log(loc, lvl, buf);
        }
    private:
        void Log(spdlog::source_loc loc, spdlog::level::level_enum lvl, const spdlog::memory_buf_t &buffer);

        static AdLog sLoggerInstance;
    };
#define AD_LOG_LOGGER_CALL(adLog, level, ...)\
        (adLog)->Log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, level, __VA_ARGS__)

#define LOG_T(...) AD_LOG_LOGGER_CALL(ade::AdLog::GetLoggerInstance(), spdlog::level::trace, __VA_ARGS__)
#define LOG_D(...) AD_LOG_LOGGER_CALL(ade::AdLog::GetLoggerInstance(), spdlog::level::debug, __VA_ARGS__)
#define LOG_I(...) AD_LOG_LOGGER_CALL(ade::AdLog::GetLoggerInstance(), spdlog::level::info, __VA_ARGS__)
#define LOG_W(...) AD_LOG_LOGGER_CALL(ade::AdLog::GetLoggerInstance(), spdlog::level::warn, __VA_ARGS__)
#define LOG_E(...) AD_LOG_LOGGER_CALL(ade::AdLog::GetLoggerInstance(), spdlog::level::err, __VA_ARGS__)
}

#endif