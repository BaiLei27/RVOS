#pragma once

#include <fstream>
#include <mutex>
#include <string_view>
#include <unordered_map>
#include <sstream>
#if __has_include(<format>)
    #include <format>
    #define LOGGER_HAS_FMT true
#else
    #define LOGGER_HAS_FMT false
#endif

#if defined(DEBUG) && !defined(NODEBUG)
    #define LOGGER_ENABLED true
#else
    #define LOGGER_ENABLED false
#endif

namespace fs= std::filesystem;

class Logger final {
public:
    enum LogLevel : int8_t {
        F, // Fatal (highest priority)
        E, // Error
        W, // Warn
        I, // Info
        D, // Debug
        T, // Trace
        A  // All
    };

private:
    std::ofstream LogFile_;

    struct LogLevelInfo {
        char lvStr_[8];
        char lvColor_[8];
    };

    using loggerLevel_u= std::unordered_map<LogLevel, LogLevelInfo>;

    loggerLevel_u LevelInfo_ {
        { F, { .lvStr_= "FATAL", .lvColor_= "\033[35m" } },
        { E, { .lvStr_= "ERROR", .lvColor_= "\033[31m" } },
        { W, { .lvStr_= "WARN", .lvColor_= "\033[33m" }  },
        { I, { .lvStr_= "INFO", .lvColor_= "\033[34m" }  },
        { D, { .lvStr_= "DEBUG", .lvColor_= "\033[32m" } },
        { T, { .lvStr_= "TRACE", .lvColor_= "\033[36m" } },
        { A, { .lvStr_= "ALL", .lvColor_= "\033[37m" }   }
    };

    std::string Timestamp_;
    std::mutex MsgLck_, CacheLck_;
    LogLevel LogLv_ { A };
    bool EnableLogFile_ { false };
    static constexpr bool G_IS_LOGGER_ENABLED= LOGGER_ENABLED;

public:
    Logger(const Logger &)            = delete;
    Logger &operator= (const Logger &)= delete;

    static Logger &GetInstance(bool enableLogFile= false);

    static void Init(LogLevel lv, bool enableLogFile= false);

    void SetLogLevel(LogLevel lv);

    std::string ExtractFuncName(std::string_view pFunc);
    std::string ExtractFuncName(const char *pFunc);

    template <typename... Args>
    void LogMsg(LogLevel lv, std::string_view funcName, int lineNum, Args &&...args)
    {
        if constexpr(!G_IS_LOGGER_ENABLED) return;

        if(lv > LogLv_) return;

        getTimestamp();
        formatTimestamp();

        // std::lock_guard<std::mutex> lock(Msg_);
        // std::lock_guard lock(Msg_);

        std::scoped_lock lock(MsgLck_); // C++17 replace lock_guard

        std::ostringstream msg;
        (msg << ... << std::forward<Args>(args));
        std::string formattedMsg;

#if LOGGER_HAS_FMT
        formattedMsg= std::format("[{}] [{}] <{}(...)[{}]> {}",
                                  Timestamp_,
                                  LevelInfo_.at(lv).lvStr_,
                                  ExtractFuncName(funcName),
                                  lineNum,
                                  msg.str());
#else
        std::ostringstream fmtStream;
        fmtStream << "[" << Timestamp_ << "] "
                  << "[" << LevelInfo_.at(lv).lvStr_ << "] "
                  << "<" << ExtractFuncName(funcName) << "[" << lineNum << "]> "
                  << msg.str();
        formattedMsg= fmtStream.str();
#endif

        if(EnableLogFile_) {
            if(!LogFile_.is_open()) {
                openLogFile();
            }
            if(LogFile_.is_open()) {
                LogFile_ << formattedMsg << '\n';
            }
        }

        logInfoOutput(lv, formattedMsg);
    }

    template <typename... Args>
    void All(std::string_view funcName, int lineNum, Args &&...args)
    {
        logWrapper<A>(funcName, lineNum, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Trace(std::string_view funcName, int lineNum, Args &&...args)
    {
        logWrapper<T>(funcName, lineNum, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Debug(std::string_view funcName, int lineNum, Args &&...args)
    {
        logWrapper<D>(funcName, lineNum, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Info(std::string_view funcName, int lineNum, Args &&...args)
    {
        logWrapper<I>(funcName, lineNum, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Warn(std::string_view funcName, int lineNum, Args &&...args)
    {
        logWrapper<W>(funcName, lineNum, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Error(std::string_view funcName, int lineNum, Args &&...args)
    {
        logWrapper<E>(funcName, lineNum, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Fatal(std::string_view funcName, int lineNum, Args &&...args)
    {
        logWrapper<F>(funcName, lineNum, std::forward<Args>(args)...);
    }

private:
    explicit Logger(bool enableLogFile);

    virtual ~Logger() noexcept;

    template <Logger::LogLevel Lv, typename... Args>
    void logWrapper(std::string_view funcName, int lineNum, Args &&...args)
    {
        if constexpr(!G_IS_LOGGER_ENABLED) return;
        LogMsg(Lv, funcName, lineNum, std::forward<Args>(args)...);
    }

    void getTimestamp();

    void formatTimestamp();

    void openLogFile();

    void logInfoOutput(Logger::LogLevel lv, std::string &msg);
};

// Macros
#if LOGGER_ENABLED
    #define LOG_INIT(lv, enableLogFile) Logger::Init(Logger::LogLevel::lv, enableLogFile)
    #define LOG_MSG(lv, msg, ...)       Logger::GetInstance().LogMsg(Logger::LogLevel::lv, __PRETTY_FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
    #define LOG_ALL(msg, ...)           Logger::GetInstance().All(__PRETTY_FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
    #define LOG_TRACE(msg, ...)         Logger::GetInstance().Trace(__PRETTY_FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
    #define LOG_DEBUG(msg, ...)         Logger::GetInstance().Debug(__PRETTY_FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
    #define LOG_INFO(msg, ...)          Logger::GetInstance().Info(__PRETTY_FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
    #define LOG_WARN(msg, ...)          Logger::GetInstance().Warn(__PRETTY_FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
    #define LOG_ERROR(msg, ...)         Logger::GetInstance().Error(__PRETTY_FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
    #define LOG_FATAL(msg, ...)         Logger::GetInstance().Fatal(__PRETTY_FUNCTION__, __LINE__, msg, ##__VA_ARGS__)
#else
    #define LOG_INIT(lv, enableLogFile)
    #define LOG_MSG(lv, msg, ...)
    #define LOG_ALL(msg, ...)
    #define LOG_TRACE(msg, ...)
    #define LOG_DEBUG(msg, ...)
    #define LOG_INFO(msg, ...)
    #define LOG_WARN(msg, ...)
    #define LOG_ERROR(msg, ...)
    #define LOG_FATAL(msg, ...)
#endif
