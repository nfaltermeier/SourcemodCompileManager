
#include "base/Compiler.h"
// https://github.com/Taywee/args
#include <args.hxx>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include "base/SCMErrors.h"
#include "formatter/Formatter.h"

void singleTest() {
    Compiler c;
    CompileResult result = c.CompileSingleFile("/home/nathan/Documents/git/hellsgamers-jailbreak/scripting", "spcomp",
                                               "hg_jbaio.sp");
    std::cout << result.status << std::endl << result.output << std::endl;
}

void directoryTest() {
    Compiler c;
    std::vector<CompileResult> results = c.CompileDirectory(
            "/home/nathan/Documents/git/hellsgamers-jailbreak/scripting", "spcomp");
    for (CompileResult &result : results) {
        std::cout << result.filename << ": " << result.status << " : "
                  << (result.endTime.count() - result.startTime.count()) <<
                  std::endl << result.output << std::endl;
    }
}

int main(int argc, char **argv) {
    args::ArgumentParser parser(
            "The Sourcemod Compile Manager. If file flag is not specified plugins are searched for in the compiler's directory.",
            "Created by Blackop778");
    //args::Group fileGroup(parser, "This group is all exclusive:", args::Group::Validators::Xor);
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Positional<std::string> compilerOption(parser, "compiler", "The path to the compiler",
                                                 args::Options::Required);
    args::ValueFlag<std::string> fileFlag(parser, "file", "What file to compile", {'f'});
    //args::ValueFlagList<char> directory(fileGroup, "dir", "What directory to compile", {'d'});
    args::Flag noHeader(parser, "noHeader", "Removes the header information from cout", {"nh"});
    args::Flag list(parser, "list", "Causes the program to list all plugins compiled", {'l'});

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
        std::cerr << "Could not find compiler at '" << compiler << "'" << std::endl;
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

    // At this point directory is the path to the directory containing the compiler and
    // compiler is the name of the compiler relative to directory
    // Debug print outs
    /*std::cout << directory << std::endl;
    std::cout << compiler << std::endl;*/

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

    //singleTest();
    //directoryTest();
}
