#include "Formatter.h"
#include <sstream>
#include <chrono>
#include <cmath>

std::string Formatter::ProcessResults(std::vector<CompileResult> results, bool noHeader, bool listPlugins) {
    std::string output;
    std::string headerInfo;
    std::string line;
    int warnings = 0;
    int errors = 0;
    int files = 0;
    int successfulFiles = 0;
    int erroredFiles = 0;
    bool errored;

    if (!noHeader) {
        headerInfo = "Sourcemod Compile Manager\n";

        std::istringstream f(results[0].output);
        while (std::getline(f, line)) {
            if (line.find("SourcePawn Compiler ") != std::string::npos) {
                headerInfo += "SourcePawn Version: " + line.substr(20) + "\n";
            }
        }
    }

    for (auto &&result : results) {
        files++;
        errored = false;
        std::istringstream f(result.output);
        std::string outBuffer = "";
        bool written = false;

        while (std::getline(f, line)) {
            if (line.find(" : warning ") != std::string::npos) {
                written = true;
                warnings++;
                outBuffer += "\t" + line + "\n";
            } else if (line.find(" : error ") != std::string::npos) {
                written = true;
                errors++;
                errored = true;
                outBuffer += "\t" + line + "\n";
            } else if (line.find(" : fatal error ") != std::string::npos) {
                written = true;
                errors++;
                errored = true;
                outBuffer += "\t" + line + "\n";
            }
        }

        if (written) {
            output += result.filename + ":\n" + outBuffer;
        } else if (listPlugins) {
            output += result.filename + ":\n\tNo warnings/errors\n";
        }

        if (!errored)
            successfulFiles++;
        else
            erroredFiles++;
    }

    // No errors were found
    if (erroredFiles == 0) {
        headerInfo += "Successfully compiled " + std::to_string(successfulFiles) + " plugins";
    } else {
        headerInfo += "Successfully compiled " + std::to_string(successfulFiles) + " plugin";

        if (successfulFiles != 1)
            headerInfo += "s";

        headerInfo += " and " + std::to_string(erroredFiles) + " with ";

        if (erroredFiles != 1)
            headerInfo += "errors";
        else
            headerInfo += "an error";
    }

    if(warnings > 0) {
        headerInfo += ". Got " + std::to_string(warnings) + " warning";

        if (warnings != 1)
            headerInfo += "s";
    }

    headerInfo += ".\n";

    // Check how long it took
    if (!noHeader) {
        std::chrono::milliseconds elapsed = results.back().endTime - results.front().startTime;

        headerInfo += "Finished in ";

        // More than 1 minute
        if (elapsed.count() > 60 * 1000) {
            // Numbers must be cast to float for division then floored and cast to int to remove decimal places
            headerInfo += std::to_string((int) floor((double) elapsed.count() / (60.0 * 1000.0))) + " mins ";
            elapsed = elapsed % (60 * 1000);
        }

        // More than 1 second
        if (elapsed.count() > 1000) {
            // Numbers must be cast to float for division then floored and cast to int to remove decimal places
            headerInfo += std::to_string((int) floor((double) elapsed.count() / 1000.0)) + " secs ";
            elapsed = elapsed % 1000;
        }

        if (elapsed.count() > 0) {
            headerInfo += std::to_string(elapsed.count()) + " ms";
        }

        headerInfo += "\n";
    }

    return headerInfo + "\n" + output;
}

