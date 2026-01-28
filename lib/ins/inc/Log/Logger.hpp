#ifndef _LOGGER_HPP_
#define _LOGGER_HPP_

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <unordered_map>
#include <regex>
#include <sstream>

#if __has_include(<format>)
    #include <format>
    #define LOGGER_HAS_FMT 1
#else
    #define LOGGER_HAS_FMT 0
#endif

#if defined(DEBUG) && !defined(NODEBUG)
    #define LOGGER_ENABLED 1
#else
    #define LOGGER_ENABLED 1
#endif

namespace fs= std::filesystem;

class Logger {
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
    std::mutex Mtx_;
    LogLevel LogLv_ { A };
    bool EnableLogFile_ { false };
    static constexpr bool G_IS_LOGGER_ENABLED= LOGGER_ENABLED;

public:
    static Logger &GetInstance(bool enableLogFile= false)
    {
        static Logger s_instance { enableLogFile };
        return s_instance;
    }

    static void Init(LogLevel lv, bool enableLogFile= false)
    {
        if constexpr(!G_IS_LOGGER_ENABLED) return;
        GetInstance(enableLogFile).SetLogLevel(lv);
    }

    void SetLogLevel(LogLevel lv)
    {
        LogLv_= lv;
    }

    static std::string ExtractFuncName(const char *pFunc)
    {
        if constexpr(!G_IS_LOGGER_ENABLED) return {};

        static std::unordered_map<std::string_view, std::string> s_cache;

        auto it= s_cache.find(pFunc);
        if(it != s_cache.end()) return it->second;

        std::regex pattern(R"((\S+)::(\S+)\s*\()");
        std::cmatch match;
        std::string result(pFunc);

        if(std::regex_search(pFunc, match, pattern)) {
            result= match.str(1) + "::" + match.str(2);
        }

        s_cache.emplace(pFunc, result);

        return result;
    }

    template <typename... Args>
    void LogMsg(LogLevel lv, const char *pFuncName, int lineNum, Args &&...args)
    {
        if(!LOGGER_ENABLED || lv > LogLv_) return;

        getTimestamp();
        formatTimestamp();
        // std::lock_guard<std::mutex> lock(Mtx_);
        // std::lock_guard lock(Mtx_);

        std::scoped_lock lock(Mtx_);

        std::string formattedMsg;
        std::ostringstream msg;
        (msg << ... << std::forward<Args>(args));

#if LOGGER_HAS_FMT
        formattedMsg= std::format("[{}] [{}] <{}(...)[{}]> {}",
                                  Timestamp_,
                                  LevelInfo_[lv].lvStr_,
                                  ExtractFuncName(pFuncName),
                                  lineNum,
                                  msg.str());
#else
        std::ostringstream fmtStream;
        fmtStream << "[" << Timestamp_ << "] "
                  << "[" << LevelInfo_[lv].lvStr_ << "] "
                  << "<" << pFuncName << "[" << lineNum << "]> "
                  << msg.str();

        formattedMsg= fmtStream.str();
#endif
        if(EnableLogFile_) {
            if(!LogFile_.is_open()) {
                openLogFile();
            }
            LogFile_ << formattedMsg << '\n';
        }

        logInfoOutput(lv, formattedMsg);
    }

    template <typename... Args>
    void All(const char *pFuncName, int lineNum, Args &&...args)
    {
        logWrapper<A>(pFuncName, lineNum, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Trace(const char *pFuncName, int lineNum, Args &&...args)
    {
        logWrapper<T>(pFuncName, lineNum, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Debug(const char *pFuncName, int lineNum, Args &&...args)
    {
        logWrapper<D>(pFuncName, lineNum, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Info(const char *pFuncName, int lineNum, Args &&...args)
    {
        logWrapper<I>(pFuncName, lineNum, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Warn(const char *pFuncName, int lineNum, Args &&...args)
    {
        logWrapper<W>(pFuncName, lineNum, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Error(const char *pFuncName, int lineNum, Args &&...args)
    {
        logWrapper<E>(pFuncName, lineNum, std::forward<Args>(args)...);
    }

    template <typename... Args>
    void Fatal(const char *pFuncName, int lineNum, Args &&...args)
    {
        logWrapper<F>(pFuncName, lineNum, std::forward<Args>(args)...);
    }

    Logger(const Logger &)            = delete;
    Logger &operator= (const Logger &)= delete;

private:
    explicit Logger(bool enableLogFile): EnableLogFile_(enableLogFile)
    {
        if constexpr(!G_IS_LOGGER_ENABLED) return;

        if(EnableLogFile_) {
            fs::create_directory("logs");
            getTimestamp();
            openLogFile();
        }
    }

    ~Logger() noexcept
    {
        if(LogFile_.is_open()) {
            LogFile_.close();
        }
    }

    template <LogLevel Lv, typename... Args>
    void logWrapper(const char *pFuncName, int lineNum, Args &&...args)
    {
        if constexpr(!G_IS_LOGGER_ENABLED) return;
        this->LogMsg(Lv, pFuncName, lineNum, std::forward<Args>(args)...);
    }

    void getTimestamp()
    {
        auto now           = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm *pLocalTime= std::localtime(&now);
        std::ostringstream pt;
        pt << std::put_time(pLocalTime, "%y-%m-%d_%H-%M-%S");
        Timestamp_= pt.str();
    }

    void formatTimestamp()
    {
        Timestamp_[8] = ' ';
        Timestamp_[11]= ':';
        Timestamp_[14]= ':';
    }

    void openLogFile()
    {
        std::string filename= "logs/" + Timestamp_ + ".log";
        LogFile_.open(filename, std::ios::out | std::ios::app);
        if(!LogFile_.is_open()) {
            std::cerr << "Unable to open log file!\n";
        }
    }

    void logInfoOutput(LogLevel lv, std::string &msg)
    {
        try {
            msg.insert(20, LevelInfo_[lv].lvColor_);
            msg.insert(msg.find(']', 20) + 1, "\033[0m");
            std::cout << msg << '\n';
        } catch(std::exception &e) {
            std::cerr << e.what() << '\n';
        }
    }
};

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

#endif /* _LOGGER_HPP_ */
