#include <iostream>
#include <fstream>
#include <format>
#include "allophone.h"
#include "exception_rom.h"

#define NAME	"CTS256A-AL2(tm) EPROM utility"
#define VERSION	"v0.1.0-alpha"

struct Options {
    std::string input_filename;
    std::string output_filename;
    uint16_t rom_address;
    size_t rom_size;
} options;

static void help()
{
	std::cerr <<
		NAME " - " VERSION "\n\n"
		"Utility to manage exception EPROM images for the CTS256-AL2 simulator.\n\n"
		"Usage:\n"
		"cts_eprom [list | encode | decode | help] [iInfile] [-oOutfile] [-aROMAddress]\n"
        "list      List the allophones used in defining exceptions\n"
        "encode    Encode the input text exception file into a binary ROM image"
        "decode    Decode the input binary ROM image to a text exception file"
		"-iInFile  Input file name\n"
		"-oOutfile Output file name\n"
        "-sSize    Size of ROM image in bytes\n"
		"-aAddress Start address of ROM image" << std::endl;
}

static bool parse_options(int argc, char* argv[])
{
    for (int i = 2; i < argc; ++i) {
        char *s = argv[i];

        if (*s == '-') {
            ++s;
            switch ( toupper( *s ) ) {
            case 'A':
                ++s;
                if (*s == ':') {
                    ++s;
                }

                try {
                    options.rom_address =
                        static_cast<uint16_t>(std::stoul( s, nullptr, 16 ));

                    if (((options.rom_address & 0xFFF ) != 0) ||
                        (options.rom_address < 0x1000) || (options.rom_address > 0xE000)) {
                        std::cerr << "Invalid EPROM address." << std::endl;
                        return false;
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << "Unrecognized address: " << s << std::endl;
                    return false;
                }
                break;
            case 'S':
                ++s;
                if (*s == ':') {
                    ++s;
                }

                try {
                    options.rom_size = std::stoul( s, nullptr, 16 );

                    if (((options.rom_size & 0xff) != 0) ||
                        (options.rom_size < 256) || (options.rom_size > 32767)) {
                        std::cerr << "Invalid EPROM address." << std::endl;
                        return false;
                    }
                }
                catch (const std::exception& e) {
                    std::cerr << "Unrecognized address: " << s << std::endl;
                    return false;
                }
                break;
            case 'I': // Input file name
                ++s;
                if ( *s == ':' ) {
                    s++;
                }
                options.input_filename = s;
                break;
            case 'O': // Output file name
                ++s;
                if ( *s == ':' ) {
                    s++;
                }
                options.output_filename = s;
                break;
            default: // Undefined switch
                std::cerr << "Unrecognized switch: -" << s << std::endl;
                std::cerr << "cts_eprom -? for help." << std::endl;
                return false;
            }
        }
    }

    return true;
}

static void list_allophones()
{
    auto header = std::format("{:^8}{:<11}{:<10}{:>10}", "Hex", "", "Sample", "");
    std::cout << header << std::endl;

    header = std::format("{:^8}{:<11}{:<10}{:>10}", "Address", "Allophone", " Word", "Duration");
    std::cout << header << std::endl;

    for (auto& allophone : allophones) {
        std::string description = std::format("   {:02X}   {:<11}{:<10}{:>10}",
            allophone.code(), allophone.name(), allophone.example(),
            allophone.duration());
        std::cout << description << std::endl;
    }
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        help();
        return 1;
    }

    std::string command{argv[1]};

    if ((command == "-?") || (command == "help")) {
        help();
        return 0;
    }
    else if (command == "list") {
        list_allophones();
        return 0;
    }
    else if (command == "encode") {
        // Set default options
        options.rom_address = 0x5000;
        options.rom_size = 4096;
        options.input_filename = "exceptions.txt";
        options.output_filename = "exception_eprom.bin";

        if (!parse_options(argc, argv)) {
            return 1;
        }

        auto exceptions = std::vector<CtsException>{};

        std::ifstream exception_file(options.input_filename);

        if (!exception_file.is_open()) {
            std::cerr << "Error opening input file: " <<
                options.input_filename << std::endl;
            return 1;
        }

        std::string line;
        while (std::getline(exception_file, line)) {
            if (line.empty()) {
                continue;
            }

            if (line[0] == ';') {
                continue;
            }

            auto exception = CtsException::from_string(line);

            exceptions.push_back(exception);
        }

        ExceptionROM rom{options.rom_address, options.rom_size};

        std::ofstream outfile(options.output_filename,
            std::ios::out | std::ios::binary);

        if (outfile.is_open()) {
            // Write the raw binary data of the vector to the file
            rom.encode(exceptions);

            outfile.write(reinterpret_cast<const char*>(rom.image().data()),
                rom.image().size());

            outfile.close();

            std::cout << "EPROM image written to " <<
                options.output_filename << std::endl;
        }
        else {
            std::cerr << "Error opening output file: " <<
                options.output_filename << std::endl;
        }
    }
    else if (command == "decode") {
        // Set default options
        options.rom_address = 0x5000;
        options.rom_size = 0;
        options.input_filename = "exception_eprom.bin";
        options.output_filename = "exceptions.txt";

        std::vector<uint8_t> rom_content;

        if (!parse_options(argc, argv)) {
            return 1;
        }

        std::ifstream rom_file(options.input_filename, std::ios::binary | std::ios::ate);

	    if (!rom_file.is_open()) {
            std::cerr << "Error opening input file: " <<
                options.input_filename << std::endl;
            return 1;
	    }

	    std::streamsize size = rom_file.tellg();

        if (options.rom_size == 0) {
            options.rom_size = size;
        }
        else if (options.rom_size < size) {
            std::cerr << "Input file is larger than " << options.rom_size <<
                "; trailing bytes will be ignored." << std::endl;
            size = options.rom_size;
        }

	    rom_file.seekg( 0, std::ios::beg );

	    rom_content.resize( size, 0xff );

	    if (!rom_file.read(reinterpret_cast<char*>(rom_content.data()), size)) {
            std::cerr << "Error reading input file: " << options.input_filename
                << std::endl;
            return 1;
        }

	    rom_file.close();

        ExceptionROM rom{options.rom_address, std::move(rom_content)};

        std::ofstream outfile(options.output_filename);

        if (outfile.is_open()) {
            auto exceptions = rom.decode();

            for (auto& exception : exceptions) {
                outfile << exception.to_string() << std::endl;
            }

            outfile.close();

            std::cout << "Exceptions written to " <<
                options.output_filename << std::endl;
        }
        else {
            std::cerr << "Error opening output file: " <<
                options.output_filename << std::endl;
        }
    }
    else {
        std::cerr << "Unrecognized command: " << command << std::endl;
        std::cerr << "cts_eprom help for help." << std::endl;
        return 1;
    }

    return 0;
}