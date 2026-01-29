#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>

namespace fs = std::filesystem;

void process_phonemes(const std::vector<char>& input, std::ostream& output)
{
    unsigned char phoneme;

    for (auto& byte : input) {
        if (byte & 0x80) {
            phoneme = byte & 0x3f;

            // Write the modified byte to the output file
            output.write(reinterpret_cast<const char*>(&phoneme), sizeof(phoneme));
        }
    }

    // Append 0xff to the end of the output file
    phoneme = 0xff;
    output.write(reinterpret_cast<const char*>(&phoneme), sizeof(phoneme));
}

int main(int argc, char* argv[])
{
    if ((argc == 1) || (argc > 3)) {
        std::cerr << "usage: to_elfos_say <input-file> [<output-file>]\n";
        return -1;
    }

    std::string arg1 = argv[1];

    if (arg1 == "-") {
        if (argc != 3) {
            std::cerr << "Must provide output file name if reading stdin.\n";
        }

        // On systems like Windows, you must reopen stdin in binary mode.
        // On Unix/POSIX systems, stdin is already binary by default.
    #if defined(_WIN32) || defined(_WIN64)
        std::freopen(nullptr, "rb", stdin);
    #endif

        // Open output file in binary mode
        std::ofstream output(argv[2], std::ios::out | std::ios::binary);
        if (!output.is_open()) {
            std::cerr << "Error opening output file: " << argv[2] << std::endl;
            return -1;
        }

        std::vector<char> data{std::istreambuf_iterator<char>(std::cin), {}};

        process_phonemes(data, output);

        output.close();
    }
    else {
        fs::path input_file = argv[1];

        if (!fs::exists(input_file)) {
            std::cerr << "Input file not found.\n";
            return -1;
        }

        fs::path output_file;

        if (argc == 3) {
            output_file = argv[2];
        }
        else {
            std::string new_extension = ".say";
            output_file = input_file;
            output_file.replace_extension(new_extension);
        }

        std::ifstream input(input_file, std::ios::in | std::ios::binary);
        if (!input.is_open()) {
            std::cerr << "Error opening input file: " << input_file << std::endl;
            return -1;
        }

        // Open output file in binary mode
        std::ofstream output(output_file, std::ios::out | std::ios::binary);
        if (!output.is_open()) {
            std::cerr << "Error opening output file: " << output_file << std::endl;
            input.close();
            return -1;
        }

        std::vector<char> data{std::istreambuf_iterator<char>(input), {}};

        process_phonemes(data, output);

        input.close();
        output.close();
    }

    return 0;
}
