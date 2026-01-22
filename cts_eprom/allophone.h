#ifndef _ALLOPHONE_H_
#define _ALLOPHONE_H_

#include <cstdint>
#include <string>
#include <string_view>
#include <chrono>
#include <array>

using Address = uint8_t;
using Duration = std::chrono::milliseconds;

class Allophone
{
public:
    Allophone(std::string_view name, Address code, std::string_view example,
        Duration duration) : name_(name), code_(code), example_(example),
        duration_(duration) {}

    std::string_view name() const { return name_; }

    Address code() const { return code_; }

    Duration duration() const { return duration_; }

    std::string_view example() const { return example_; }

private:
    const std::string name_;
    const Address code_;
    const std::string example_;
    const Duration duration_;
};

extern const std::array<const Allophone, 64> allophones;

#endif /* _ALLOPHONE_H_ */