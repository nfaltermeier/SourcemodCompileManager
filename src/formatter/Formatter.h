/*
 * Formatter.h
 *
 *  Created on: Apr 23, 2018
 *      Author: nathan
 */

#ifndef SRC_FORMATTER_FORMATTER_H_
#define SRC_FORMATTER_FORMATTER_H_

#include "../base/CompileResult.h"
#include <iostream>
#include <vector>

class Formatter {
public:
	std::string ProcessResults(std::vector<CompileResult>);
};

#endif /* SRC_FORMATTER_FORMATTER_H_ */
