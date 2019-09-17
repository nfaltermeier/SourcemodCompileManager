
// https://github.com/Taywee/args
#include <args.hxx>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>

#include "base/Compiler.h"
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
    args::ValueFlag<std::string> compileArgsFlag(parser, "compilerArgs", "Arguments to pass to the compiler", {"compilerArgs"});
    args::ValueFlag<std::string> outputDirectoryFlag(parser, "outputDirectory", "Every file compiled will be placed in this directory", {"outputDirectory"});

    try {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help&) {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (args::ValidationError &e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    std::string compiler = compilerOption.Get();

    if (access(compiler.c_str(), F_OK) != 0) {
        std::cerr << "'" << compiler << "' does not exist (could not find compiler)" << std::endl;
        return 1;
    }

    struct stat compilerStat{};
    if (stat(compiler.c_str(), &compilerStat) != 0) {
        std::cerr << "Something went wrong when checking access to the compiler at '" << compiler << "'" << std::endl;
        return 1;
    }

    // path does not specify a file
    if (!(compilerStat.st_mode & S_IFREG)) {
        std::cerr << "'" << compiler << "' (the compiler) does not specify a file" << std::endl;
        return 1;
    }

    if (access(compiler.c_str(), X_OK) != 0) {
        std::cerr << "No permission to execute compiler at '" << compiler << "'" << std::endl;
        return 1;
    }

    std::string outputDirectory;
    if (outputDirectoryFlag.Matched()) {
        outputDirectory = outputDirectoryFlag.Get();

        struct stat outputDirectoryStat{};
        if (stat(outputDirectory.c_str(), &outputDirectoryStat) != 0) {
            std::cerr << "Something went wrong when checking access to the output directory at '" << outputDirectory << "'" << std::endl;
            return 1;
        }

        // path does not specify a file
        if (!(outputDirectoryStat.st_mode & S_IFDIR)) {
            std::cerr << "'" << outputDirectory << "' (the output directory) does not specify a directory" << std::endl;
            return 1;
        }

        if (access(outputDirectory.c_str(), W_OK) != 0) {
            std::cerr << "No permission to write to output directory at '" << outputDirectory << "'" << std::endl;
            return 1;
        }

        // make sure outputDirectory ends in a slash
        if (outputDirectory.back() != '/') {
            outputDirectory.push_back('/');
        }
    }

    std::string directory;
    ulong pos = compiler.find_last_of({'/', '\0'});
    // Could not find a forward slash
    if (pos == std::string::npos) {
        char currentPath[FILENAME_MAX];
        getcwd(currentPath, sizeof(currentPath));
        directory = currentPath;
    } else {
        directory = compiler.substr(0, pos + 1);
        compiler = compiler.substr(pos + 1);
    }

    std::vector<CompileResult> results;
    std::string compilerArgs;
    if (compileArgsFlag.Matched()) {
        compilerArgs = compileArgsFlag.Get();
    }

    // Are we compiling a file or not?
    if (fileFlag.Matched()) {
        std::string file = fileFlag.Get();
        std::string path = directory + file;

        if (access(path.c_str(), F_OK) != 0) {
            std::cerr << "Could not find file to compile at '" << path << "'" << std::endl;
            return 1;
        }

        results.push_back(Compiler::CompileSingleFile(directory, compiler, file, compilerArgs, outputDirectory));
    } else {
        results = Compiler::CompileDirectory(directory, compiler, compilerArgs, outputDirectory);
    }

    std::cout << Formatter::ProcessResults(results, noHeader, list) << std::endl;
}
