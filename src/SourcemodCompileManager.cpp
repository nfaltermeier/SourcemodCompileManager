
#include "SourcemodCompileManager.h"
#include "base/Compiler.h"
// https://github.com/Taywee/args
#include <args.hxx>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include "base/SCMErrors.h"

void singleTest()
{
	Compiler c;
	CompileResult result = c.CompileSingleFile("/home/nathan/Documents/git/hellsgamers-jailbreak/scripting", "spcomp", "hg_jbaio.sp");
	std::cout << result.status << std::endl << result.output << std::endl;
}

void directoryTest()
{
	Compiler c;
	std::vector<CompileResult> results = c.CompileDirectory("/home/nathan/Documents/git/hellsgamers-jailbreak/scripting", "spcomp");
	for (CompileResult &result : results)
	{
		std::cout << result.filename << ": " << result.status << " : " << (result.endTime.count() - result.startTime.count()) <<
				std::endl << result.output << std::endl;
	}
}

int main(int argc, char **argv) {
	std::cout << &SCMErrors::linkFail << std::endl;
	args::ArgumentParser parser("This is a test program", "this goes after options");
	//args::Group fileGroup(parser, "This group is all exclusive:", args::Group::Validators::Xor);
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlag<std::string> fileFlag(parser, "file", "What file to compile", {'f'});
	//args::ValueFlagList<char> directory(fileGroup, "dir", "What directory to compile", {'d'});
	args::Positional<std::string> compilerFlag(parser, "compiler", "The path to the compiler");
	try
	{
		parser.ParseCLI(argc, argv);
	}
	catch(args::Help)
	{
		std::cout << parser;
		return 0;
	}
	catch(args::ParseError e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}
	catch(args::ValidationError e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << parser;
		return 1;
	}

	std::string compiler = compilerFlag.Get();

	if(access(compiler.c_str(), F_OK) != 0)
	{
		std::cerr << "Could not find compiler at '" << compiler << "'" << std::endl;
		return 1;
	}

	if(access(compiler.c_str(), X_OK) != 0)
	{
		std::cerr << "No permission to execute compiler at '" << compiler << "'" << std::endl;
		return 1;
	}

	std::string directory;
	uint pos = compiler.find_last_of({'/', (char)0});
	// Could not find a forward slash
	if(pos == std::string::npos)
	{

	}
	else
	{
		directory = compiler.substr(0, pos + 1);
		compiler = compiler.substr(pos + 1);
	}

	// At this point directory is the path to the directory containing the compiler and
	// compiler is the name of the compiler relative to directory
	std::cout << directory << std::endl;
	std::cout << compiler << std::endl;

	// Are we compiling a file or not?
	if(!fileFlag.Matched())
	{
		Compiler c;
		std::vector<CompileResult> results = c.CompileDirectory(directory, compiler);
		for (CompileResult &result : results)
		{
			std::cout << result.filename << ": " << result.status << " : " << (result.endTime.count() - result.startTime.count()) <<
					std::endl << result.output << std::endl;
		}
	}
	else
	{
		std::string file = fileFlag.Get();
		std::string path = directory + file;
		if(access(path.c_str(), F_OK) != 0)
		{
			std::cerr << "Could not find file to compile at '" << path << "'" << std::endl;
			return 1;
		}
		Compiler c;
		CompileResult result = c.CompileSingleFile(directory, compiler, file);
		std::cout << result.status << std::endl << result.output << std::endl;
	}

	//singleTest();
	//directoryTest();
}
