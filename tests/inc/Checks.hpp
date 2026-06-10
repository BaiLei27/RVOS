#pragma once

#include <cstdlib>
#include <print>
#include <string_view>

namespace tst {

class Checks {
public:
    void Check(bool cond, std::string_view msg)
    {
        ++Total_;
        if(cond) {
            ++Passed_;
            return;
        }
        ++Failed_;
        std::println("  FAIL: {}", msg);
    }

    template <typename... Args>
    void Check(bool cond, std::format_string<Args...> fmt, Args &&...args)
    {
        ++Total_;
        if(cond) {
            ++Passed_;
            return;
        }
        ++Failed_;
        std::println("  FAIL: {}", std::format(fmt, std::forward<Args>(args)...));
    }

    [[nodiscard]] bool Report(std::string_view suite) const
    {
        std::println("{}: {} cases, {} passed, {} failed", suite, Total_, Passed_, Failed_);

        if(Failed_ == 0) {
            std::println("PASS: {}", suite);
            return true;
        }

        std::println("FAIL: {}", suite);
        return false;
    }

    [[noreturn]] void Exit(std::string_view suite) const
    {
        std::exit(Report(suite) ? EXIT_SUCCESS : EXIT_FAILURE);
    }

    [[nodiscard]] int Total() const { return Total_; }

    [[nodiscard]] int Passed() const { return Passed_; }

    [[nodiscard]] int Failed() const { return Failed_; }

private:
    int Total_ {};
    int Passed_ {};
    int Failed_ {};
};

} // namespace tst
