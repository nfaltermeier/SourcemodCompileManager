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
	std::string output;

	for(CompileResult result : results)
	{
		std::istringstream f(result.output);
		std::string line;
		while (std::getline(f, line)) {
			std::cout << line << std::endl;
		}
	}

	return output;
}

