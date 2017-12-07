/*
 * Compiler.h
 *
 *  Created on: Oct 24, 2017
 *      Author: nsfaltermeier
 */

#ifndef BASE_COMPILER_H_
#define BASE_COMPILER_H_

#include "CompileResult.h"
#include <iostream>
#include <vector>

class Compiler {
public:
	Compiler();
	virtual ~Compiler();
	CompileResult CompileSingleFile(std::string, std::string, std::string);
	std::vector<CompileResult> CompileDirectory(std::string, std::string);
	bool StrEndsWith(std::string, std::string);
};

#endif /* BASE_COMPILER_H_ */
