#include "decoder.h"
#include "encoder.h"
#include "parser.h"

#include <cstdint>
#include <iostream>

const char* const HELP_INFO =
    "OPTIONS:\n"
    " -c archive_name file1 [file2 ...] -> archive files: file1, file2, ..."
    "and save result to file named: archive_name.\n"
    " -d archive_name -> de-archive files from archive_name and place in current directory\n"
    " -h -> display help\n";

const char* const CALL_ENCODE = "-c";
const char* const CALL_DECODE = "-d";
const char* const CALL_HELP = "-h";

void Help() {
    std::cout << HELP_INFO << std::endl;
}

int main(int32_t argc, char** argv) {

    try {
        ArgParser parser = ArgParser();

        parser.AddArgument(CALL_ENCODE, Action::Extend);
        parser.AddArgument(CALL_DECODE, Action::StoreValue);
        parser.AddArgument(CALL_HELP, Action::Flag);

        parser.ParseArguments(argc, argv);

        auto parsed_args = parser.GetAllParsedArguments();

        if (parsed_args.size() != 1) {
            throw std::invalid_argument("Unrecognized call type");
        }

        if (parser.CheckParsed(CALL_ENCODE)) {
            std::vector<std::string> filenames = parser.GetExtendedArgument(CALL_ENCODE);
            std::string archive_name = filenames[0];
            filenames.erase(filenames.begin());
            Encoder(filenames, archive_name);
        }
        if (parser.CheckParsed(CALL_DECODE)) {
            Decoder(parser.GetArgument(CALL_DECODE));
        }
        if (parser.CheckParsed(CALL_HELP)) {
            Help();
        }

    } catch (std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return constants::FAILED_EXIT_CODE;
    }

    return 0;
}
