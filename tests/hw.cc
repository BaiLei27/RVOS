#include <list>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <memory>
extern void uart_init();
extern int uart_putc(char ch);
extern void uart_puts(const char *pS);
extern void uart_put_uint(uint32_t num);
extern void uart_put_int(int32_t num);
extern void uart_put_int(long num);
extern void uart_put_hex(uint32_t num);
extern void uart_put_long_hex(uint64_t num);

extern "C" {
    void _exit(int status)
    {
        (void)status;

        // Ensure all prior memory operations are completed before entering halt state
        asm volatile("" ::: "memory");

        // Halt cpu by entering infinite loop
        while(true) {
            asm volatile("wfi");
        }
    }
}

int main()
{
    using namespace std::literals;
    uart_init();
    uart_puts("Testing STL std::list, std::vector, std::unordered_map on RISC-V with Picolibc\n");
    std::list<std::string_view> l { "list\n"sv, "RISC-V\n"sv };
    std::vector<std::string_view> v { "vector\n"sv, "RISC-V\n"sv };
    std::unordered_map<int32_t, std::string_view> m {
        { 32, "RISC-V:32\n"sv },
        { 64, "RISC-V:64\n"sv }
    };

    std::unique_ptr<std::list<std::string_view>> pL  = std::make_unique<std::list<std::string_view>>(l);
    std::shared_ptr<std::vector<std::string_view>> pV= std::make_shared<std::vector<std::string_view>>(v);

    uart_puts(pL->front().data());
    uart_puts(pV->front().data());

    for(const auto &c: l) {
        uart_puts(c.data());
    }

    for(const auto &c: v) {
        uart_puts(c.data());
    }

    for(auto &[k, v]: m) {
        uart_put_int(k);
        uart_puts(" -> ");
        uart_puts(v.data());
    }

    return 0;
}
