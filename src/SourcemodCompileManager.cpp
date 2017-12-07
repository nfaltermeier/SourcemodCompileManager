
#include "SourcemodCompileManager.h"
// https://github.com/Taywee/args
#include "base/Compiler.h"
#include <args.hxx>
#include <iostream>
#include <chrono>

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
	args::ArgumentParser parser("This is a test program", "this goes after options");
	//args::Group fileGroup(parser, "This group is all exclusive:", args::Group::Validators::Xor);
	args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
	args::ValueFlagList<char> file(parser, "file", "What file to compile", {'f'});
	//args::ValueFlagList<char> directory(fileGroup, "dir", "What directory to compile", {'d'});
	args::PositionalList<std::string> compiler(parser, "compiler", "The path to the compiler");
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
	//singleTest();
	directoryTest();
}
