/**
 * @file Logger.cc
 * @author Swan BaiLei27 (https://github.com/BaiLei27)
 * @brief
 * @version 0.1
 * @date 2026/03/30/10:03
 *
 * @copyright Copyright (c) 2026 Swan BaiLei27 <SwanBaiLei2Seven@foxmail.com>
 *
 */

#include "Logger.hh"

#include <iomanip>
#include <iostream>
#include <filesystem>
#include <regex>

// Static cache for function name extraction

std::string Logger::ExtractFuncName(const char *pFunc)
{
    static std::unordered_map<std::string_view, std::string> s_cache;
    {
        std::scoped_lock lock(CacheLck_);
        auto it= s_cache.find(pFunc);
        if(it != s_cache.end()) return it->second;
    }

    std::regex pattern(R"((\S+)::(\S+)\s*\()");
    std::cmatch match;
    std::string result(pFunc);

    if(std::regex_search(pFunc, match, pattern)) {
        result= match.str(1) + "::" + match.str(2);
    }

    {
        std::scoped_lock lock(CacheLck_);
        s_cache.emplace(pFunc, result);
    }

    return result;
}

std::string Logger::ExtractFuncName(std::string_view funcName)
{
    // std::cout << funcName << '\n';
    // return {};
    static std::unordered_map<std::string_view, std::string> s_FuncNameCache;

    {
        std::scoped_lock lock(CacheLck_);
        auto it= s_FuncNameCache.find(funcName);
        if(it != s_FuncNameCache.end()) {
            return it->second;
        }
    }

    std::string res(funcName);
    auto openPos= funcName.find('(');
    if(openPos != std::string::npos) {
        res= funcName.substr(0, openPos);
    }

    {
        std::scoped_lock lock(CacheLck_);
        s_FuncNameCache.emplace(funcName, res);
    }

    // std::cout << "Cache size:" << s_FuncNameCache.size() << '\n';
    // for(auto &[k, v]: s_FuncNameCache) {
    //     std::cout << k << " -> " << v << '\n';
    // }

    return res;
}

Logger &Logger::GetInstance(bool enableLogFile)
{
    static Logger s_instance(enableLogFile);
    return s_instance;
}

void Logger::Init(LogLevel lv, bool enableLogFile)
{
    if constexpr(!G_IS_LOGGER_ENABLED) return;
    GetInstance(enableLogFile).SetLogLevel(lv);
}

void Logger::SetLogLevel(LogLevel lv)
{
    LogLv_= lv;
}

Logger::Logger(bool enableLogFile): EnableLogFile_(enableLogFile)
{
    if(EnableLogFile_) {
        fs::create_directories("logs");
        getTimestamp();
        openLogFile();
    }
}

Logger::~Logger() noexcept
{
    if(LogFile_.is_open()) {
        LogFile_.close();
    }
}

void Logger::getTimestamp()
{
    auto now           = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm *pLocalTime= std::localtime(&now);
    std::ostringstream pt;
    pt << std::put_time(pLocalTime, "%y-%m-%d_%H-%M-%S");
    Timestamp_= pt.str();
}

void Logger::formatTimestamp()
{
    if(Timestamp_.size() >= 15) {
        Timestamp_[8] = ' ';
        Timestamp_[11]= ':';
        Timestamp_[14]= ':';
    }
}

void Logger::openLogFile()
{
    std::string filename= "logs/" + Timestamp_ + ".log";
    LogFile_.open(filename, std::ios::out | std::ios::app);
    if(!LogFile_.is_open()) {
        std::cerr << "Unable to open log file: " << filename << "\n";
    }
}

void Logger::logInfoOutput(LogLevel lv, std::string &msg)
{
    try {
        auto &color= LevelInfo_.at(lv).lvColor_;
        using namespace std::string_view_literals;
        std::string_view resetColor= "\033[0m"sv;

        auto openPos = msg.find('[', msg.find(']')); // find second bracket
        auto closePos= msg.find(']', openPos);

        msg.insert(closePos + 1, resetColor); //! insert reset color first
        msg.insert(openPos, color);

        std::cout << msg << '\n';
    } catch(const std::exception &e) {
        std::cerr << "Log output error: " << e.what() << '\n';
    }
}

// Explicit instantiations for common use (optional but recommended to avoid linker errors)
template void Logger::LogMsg<int>(LogLevel, std::string_view, int, int &&);
template void Logger::LogMsg<std::string_view>(LogLevel, std::string_view, int, std::string_view &&);
template void Logger::LogMsg<std::string>(LogLevel, std::string_view, int, std::string &&);
