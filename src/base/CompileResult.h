/*
 * CompileResult.h
 *
 *  Created on: Nov 16, 2017
 *      Author: nathan
 */

#ifndef BASE_COMPILERESULT_H_
#define BASE_COMPILERESULT_H_

#include <iostream>
#include <chrono>

struct CompileResult {
    int status;
    std::string output;
    std::string filename;
    std::chrono::milliseconds startTime;
    std::chrono::milliseconds endTime;
};

#endif /* BASE_COMPILERESULT_H_ */
