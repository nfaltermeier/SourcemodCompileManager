/*
 * Formatter.cpp
 *
 *  Created on: Apr 23, 2018
 *      Author: nathan
 */

#include "Formatter.h"
#include <sstream>

std::string Formatter::ProcessResults(std::vector<CompileResult> results)
{
	std::string output = "Sourcemod Compile Manager\n";
	std::istringstream f(results[0].output);
	std::string line;
	int warnings;
	int errors;
	while (std::getline(f, line)) {
		if(line.find("SourcePawn Compiler ") != std::string::npos) {
			output += "Sourcemod Version: " + line.substr(20);
		}
	}

	for(CompileResult result : results)
	{
		std::istringstream f(result.output);
		std::string line;
		while (std::getline(f, line)) {
			if(line.find(" : warning ")) {

			} else if(line.find(" : error ")) {

			}
		}
	}

	return output;
}

