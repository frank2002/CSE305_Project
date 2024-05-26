/*
 * Author: doe300
 *
 * See the file "LICENSE" for the full license governing this code.
 */
#ifndef CPPLOG_LOGGER_H
#define CPPLOG_LOGGER_H

#include "log.h"

#include <chrono>
#include <fstream>
#include <mutex>
#include <string>
#include <thread>
#include <iostream>
#include <iomanip>


namespace cpplog
{
    class Logger
    {
    public:
        Logger(const Logger&) = delete;
        Logger(Logger&&) = delete;
        virtual ~Logger() = default;

        Logger& operator=(const Logger&) = delete;
        Logger& operator=(Logger&&) = delete;

        /*
         * Writes the log to the output beneath
         *
         * NOTE: Implementations of this method need to be thread-safe, e.g. this method can be written to from multiple
         * threads concurrently
         */
        virtual void logMessage(
            Level level, const std::wstring& local, std::chrono::system_clock::time_point timestamp) = 0;

        /*
         * Whether a log text with this level will be logged, e.g. the given level is larger or equals minLevel
         */
        bool willBeLogged(Level level) const;

    protected:
        std::string toString(std::chrono::system_clock::time_point timestamp) const;
        std::wstring toString(Level level) const;

        explicit Logger(Level minimumLevel = Level::INFO) noexcept;

        std::mutex writeLock;
        Level minLevel;
    };

    class ConsoleLogger : public Logger
    {
    public:
        explicit ConsoleLogger(Level minimumLevel = Level::INFO) noexcept;
        ConsoleLogger(const ConsoleLogger&) = delete;
        ConsoleLogger(ConsoleLogger&&) = delete;
        ~ConsoleLogger() override = default;

        ConsoleLogger& operator=(const ConsoleLogger&) = delete;
        ConsoleLogger& operator=(ConsoleLogger&&) = delete;

        void logMessage(
            Level level, const std::wstring& local, std::chrono::system_clock::time_point timestamp) override;
    };

    class FileLogger : public Logger
    {
    public:
        explicit FileLogger(const std::string& fileName, Level minimumLevel = Level::INFO);
        FileLogger(const FileLogger&) = delete;
        FileLogger(FileLogger&&) = delete;
        ~FileLogger() override;

        FileLogger& operator=(const FileLogger&) = delete;
        FileLogger& operator=(FileLogger&&) = delete;

        void logMessage(
            Level level, const std::wstring& local, std::chrono::system_clock::time_point timestamp) override;

    private:
        std::wofstream fileStream;
    };


    // Customized logger for better output format
    class CustomLogger : public Logger {
    public:
        CustomLogger(Level minimumLevel) noexcept : Logger(minimumLevel) {}

    protected:
        void logMessage(cpplog::Level level, const std::wstring& message, std::chrono::system_clock::time_point timestamp) override {
            if (!willBeLogged(level))
                return;

            std::lock_guard<std::mutex> guard(writeLock);

            std::wstringstream oss;

            // Get current time
            auto t = std::chrono::system_clock::to_time_t(timestamp);
            auto tm = *std::localtime(&t);

            // Format time
            oss << L"[" << toString(level) << L"]";
            oss << L"[" << std::put_time(&tm, L"%Y/%m/%d %H:%M:%S") << L"]";

            // Add module

            std::string fileName = __FILE__;
            fileName = fileName.substr(fileName.find_last_of("/\\") + 1);
            fileName = fileName.substr(0, fileName.find_last_of("."));
            std::wstring mod = std::wstring(fileName.begin(), fileName.end());
            oss << L"[" << mod << L"]";

            // Add thread id
            oss << L"[T:" << std::this_thread::get_id() << L"] ";

            // Add log message
            oss << message;

            // Output to console
            std::wstring logMessage = oss.str();
            std::wcout << logMessage;
        }

    private:
        std::wstring toString(cpplog::Level level) const {
            switch (level) {
                case cpplog::Level::DEBUG: return L"\033[0mDEBUG\033[0m";  // Default
                case cpplog::Level::INFO: return L"\033[32mINFO\033[0m";    // Green
                case cpplog::Level::WARNING: return L"\033[33mWARN\033[0m"; // Yellow
                case cpplog::Level::ERROR: return L"\033[31mERROR\033[0m";  // Red
                case cpplog::Level::SEVERE: return L"\033[31mSEVERE\033[0m"; // Red
                default: return L"\033[0mUNKNOWN\033[0m"; // Default
            }
        }
    };

    class StreamLogger : public Logger
    {
    public:
        explicit StreamLogger(std::wostream& out, Level minimumLevel = Level::INFO);
        StreamLogger(const StreamLogger&) = delete;
        StreamLogger(StreamLogger&&) = delete;
        ~StreamLogger() override = default;

        StreamLogger& operator=(const StreamLogger&) = delete;
        StreamLogger& operator=(StreamLogger&&) = delete;

        void logMessage(
            Level level, const std::wstring& local, std::chrono::system_clock::time_point timestamp) override;

    protected:
        std::wostream& stream;
    };

    class ColoredLogger : public StreamLogger
    {
    public:
        explicit ColoredLogger(std::wostream& out, Level minimumLevel = Level::INFO);
        ColoredLogger(const ColoredLogger&) = delete;
        ColoredLogger(ColoredLogger&&) = delete;
        ~ColoredLogger() override = default;

        ColoredLogger& operator=(const ColoredLogger&) = delete;
        ColoredLogger& operator=(ColoredLogger&&) = delete;

        void logMessage(
            Level level, const std::wstring& local, std::chrono::system_clock::time_point timestamp) override;
    };
} // namespace cpplog

#endif /* CPPLOG_LOGGER_H */
