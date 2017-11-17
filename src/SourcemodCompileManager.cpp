
#include "SourcemodCompileManager.h"
// https://github.com/Taywee/args
#include <args.hxx>
#include <iostream>

void singleTest()
{
	Compiler c;
	CompileResult result = c.CompileSingleFile("/home/nathan/Documents/git/hellsgamers-jailbreak/scripting", "spcomp", "hg_jbaio.sp");
	std::cout << result.output << std::endl;
}

void directoryTest()
{
	Compiler c;
	std::vector<CompileResult> results = c.CompileDirectory("/home/nathan/Documents/git/hellsgamers-jailbreak/scripting", "spcomp");

}

int main() {
	singleTest();
}
