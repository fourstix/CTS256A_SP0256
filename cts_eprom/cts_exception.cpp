#include <regex>
#include <algorithm>
#include <cctype>
#include "allophone.h"
#include "cts_exception.h"

const std::regex CtsException::exception_regex{
    R"(^\s*(\<?)\s*([^\[\s]*)\[([^\]]*)\]([^\<\s]*)\s*(\<?))"
    R"(\s*=\s*\[([^\]]+)\]\s*$)"};

const std::regex CtsException::allophone_regex{R"([A-Z]{2}[1-5]?)"};

CtsException::CtsException(const CtsTag tag,
    const std::vector<uint8_t>& encoding) :
        tag_(tag), encoded_(encoding)
{

}

CtsException CtsException::from_string(std::string& line)
{
    std::transform(line.begin(), line.end(), line.begin(),
        [](unsigned char c) { return std::toupper(c); });

    std::vector<uint8_t> encoded;
    std::smatch matches;

    if (std::regex_match(line, matches, exception_regex)) {
        if (matches[1].compare("<") == 0) {
            encoded.push_back(0x13);
        }

        for (auto it = matches[2].first; it != matches[2].second; it++) {
            encoded.push_back(CtsTag(*it).to_code());
        }

        auto word = matches[3];

        auto tag  = CtsTag{*(word.first)};

        if (word.length() == 1) {
            if (std::isalpha(tag.symbol)) {
                encoded.push_back(0xff);
            }
            else {
                encoded.push_back(tag.to_code() | 0x40 | 0x80);
            }
        }
        else {
            for (auto it = word.first + 1; it != word.second; it++) {
                uint8_t symbol = *it;
                uint8_t flags = 0;

                if (it == (word.first + 1)) {
                    flags |= 0x40;
                }

                if (std::next(it) == word.second) {
                    flags |= 0x80;
                }

                encoded.push_back(CtsTag(symbol).to_code() | flags);
            }
        }

        for (auto it = matches[4].first; it != matches[4].second; it++) {
            encoded.push_back(CtsTag(*it).to_code());
        }

        if (matches[5].compare("<") == 0) {
            encoded.push_back(0x13);
        }

        auto allophone_list = matches[6].str();

        bool first = true;

        for (std::smatch sm; regex_search(allophone_list, sm, allophone_regex);) {
            auto name = sm.str();
            auto it = std::find_if(allophones.begin(), allophones.end(),
                [name](const Allophone& allophone) {
                    return allophone.name() == name;
            });

            if (it != allophones.end()) {
                auto code = it->code();
                if (first) {
                    code |= 0x40;
                    first = false;
                }
                encoded.push_back(code);
            }

            allophone_list = sm.suffix();
        }

        encoded.back() |= 0x80;

        return CtsException{tag, encoded};
    }
    else {
        throw std::runtime_error("Bad input");
    }
}

std::string CtsException::to_string()
{
    std::string output;
    auto it = encoded_.begin();

    if (*it == 0x13) {
        output.push_back('<');
        it++;
    }

    while ((*it & 0x40) == 0) {
        output.push_back(CtsTag::from_code(*it).symbol);
        it++;
    }

    output.push_back('[');

    if (std::isalpha(tag_.symbol)) {
        output.push_back(tag_.symbol);
    }

    if ((*it & 0x80) != 0) {
        if (*it != 0xff) {
            output.push_back(CtsTag::from_code(*it & ~(0x40 | 0x80)).symbol);
        }
        it++;
    }
    else {
        while ((*it & 0x80) == 0) {
            output.push_back(CtsTag::from_code(*it & ~0x40).symbol);
            it++;
        }

        output.push_back(CtsTag::from_code(*it & ~(0x40 | 0x80)).symbol);
        it++;
    }

    output.push_back(']');

    while ((*it != 0x13) && ((*it & 0x40) == 0)) {
        output.push_back(CtsTag::from_code(*it).symbol);
        it++;
    }

    if (*it == 0x13) {
        output.push_back('<');
        it++;
    }

    output.append(" = [");

    bool first = true;
    while (it != encoded_.end()) {
        if (!first) {
            output.push_back(' ');
        }
        else {
            first = false;
        }
        output.append(allophones[*it & ~(0x40 | 0x80)].name());
        it++;
    }

    output.push_back(']');

    return output;
}