#ifndef _CTS_EXCEPTION_H_
#define _CTS_EXCEPTION_H_

#include <string>
#include <regex>

struct CtsTag {
    const char symbol;

    static CtsTag from_code(uint8_t code) {
        return CtsTag{static_cast<char>(code + 0x20)};
    }

    uint8_t to_code() const { return symbol - 0x20; }

    int to_index() const { return std::isalpha(symbol) ? symbol - 0x41 : 26; }
};

class CtsException {
public:
    CtsException(const CtsTag tag, const std::vector<uint8_t>& encoding);

    static CtsException from_string(std::string& input);

    const CtsTag& tag() const { return tag_; }

    const std::vector<uint8_t>& to_bytes() { return encoded_; }

    std::string to_string();

private:
    static const std::regex exception_regex;
    static const std::regex allophone_regex;

    const CtsTag tag_;
    const std::vector<uint8_t> encoded_;
};

#endif /* _CTS_EXCEPTION_H_ */