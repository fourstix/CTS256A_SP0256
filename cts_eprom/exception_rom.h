#ifndef _EXCEPTION_ROM_H_
#define _EXCEPTION_ROM_H_

#include <cstdint>
#include <vector>
#include "cts_exception.h"

class ExceptionROM {
public:
    ExceptionROM(uint16_t address, size_t rom_size);

    ExceptionROM(uint16_t addresss, std::vector<uint8_t>&& content);

    static uint8_t code_for(char c) { return c  - 0x20; }

    void encode(const std::vector<CtsException>& exceptions);

    const std::vector<CtsException> decode();

    const std::vector<uint8_t>& image() { return rom_; }

private:
    template<typename TIterator>
    uint16_t address_of(TIterator it);

    uint16_t address_;
    std::vector<uint8_t> rom_;
};

#endif /* _EXCEPTION_ROM_H_ */