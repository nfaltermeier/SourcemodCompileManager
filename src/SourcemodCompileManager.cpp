
// https://github.com/Taywee/args
#include <args.hxx>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <sys/stat.h>

#include "base/Compiler.h"
#include "base/SCMErrors.h"
#include "formatter/Formatter.h"

int main(int argc, char **argv) {
    args::ArgumentParser parser(
            "The Sourcemod Compile Manager. If file flag is not specified plugins are searched for in the compiler's directory.",
            "Created by Blackop778");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Positional<std::string> compilerOption(parser, "compiler", "The path to the compiler",
                                                 args::Options::Required);
    args::ValueFlag<std::string> fileFlag(parser, "file", "What file to compile", {'f', "file"});
    args::Flag noHeader(parser, "noHeader", "Removes the header information from cout", {"nh", "noHeader"});
    args::Flag list(parser, "list", "Causes the program to list all plugins compiled", {'l', "list"});

    try {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help) {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (args::ValidationError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    std::string compiler = compilerOption.Get();

    if (access(compiler.c_str(), F_OK) != 0) {
        std::cerr << "'" << compiler << "' does not exist (could not find compiler)" << std::endl;
        return 1;
    }

    struct stat compilerStat;
    if (stat(compiler.c_str(), &compilerStat) != 0) {
        std::cerr << "Something went wrong when checking access to the compiler at '" << compiler << "'" << std::endl;
        return 1;
    }

    // path does not specify a file
    if (!(compilerStat.st_mode & S_IFREG)) {
        std::cerr << "'" << compiler << "' does not specify a file" << std::endl;
        return 1;
    }

    if (access(compiler.c_str(), X_OK) != 0) {
        std::cerr << "No permission to execute compiler at '" << compiler << "'" << std::endl;
        return 1;
    }

    std::string directory;
    ulong pos = compiler.find_last_of({'/', (char) 0});
    // Could not find a forward slash
    if (pos == std::string::npos) {
        char currentPath[FILENAME_MAX];
        getcwd(currentPath, sizeof(currentPath));
        directory = currentPath;
    } else {
        directory = compiler.substr(0, pos + 1);
        compiler = compiler.substr(pos + 1);
    }

    Compiler c;
    std::vector<CompileResult> results;

    // Are we compiling a file or not?
    if (fileFlag.Matched()) {
        std::string file = fileFlag.Get();
        std::string path = directory + file;

        if (access(path.c_str(), F_OK) != 0) {
            std::cerr << "Could not find file to compile at '" << path << "'" << std::endl;
            return 1;
        }

        results.push_back(c.CompileSingleFile(directory, compiler, file));
    } else {
        results = c.CompileDirectory(directory, compiler);
    }

    Formatter f;
    std::cout << f.ProcessResults(results, noHeader, list) << std::endl;
}
