#pragma once

/// @brief CRTP singleton base. Derive as `class Foo : public util::ISingleton<Foo>` and add
/// `explicit Foo(Access a) : ISingleton<Foo>(a) {}`.
template <typename Derived>
class ISingleton {
public:
    ISingleton(const ISingleton &)            = delete;
    ISingleton(ISingleton &&)                 = delete;
    ISingleton &operator= (const ISingleton &)= delete;
    ISingleton &operator= (ISingleton &&)     = delete;

    static Derived &Instance()
    {
        static Derived s_inst { Access {} };
        return s_inst;
    }

protected:
    struct Access {
        constexpr Access()= default;
    };

    explicit ISingleton(Access /*unused*/) { }

    virtual ~ISingleton()= default;
};
