/*
 * Compiler.cpp
 *
 *  Created on: Oct 24, 2017
 *      Author: nsfaltermeier
 */

#include "Compiler.h"
#include <unistd.h>
#include <sys/wait.h>
#include <cerrno>
#include <dirent.h>
#include <chrono>
#include <sys/mman.h>
#include "SCMErrors.h"
#include <cstring>


Compiler::Compiler() {
    // TODO Auto-generated constructor stub

}

Compiler::~Compiler() {
    // TODO Auto-generated destructor stub
}

CompileResult Compiler::CompileSingleFile(std::string directory,
                                          std::string executable, std::string fileToCompile) {
    int link[2];
    pid_t pid;
    CompileResult result;
    result.status = 0;

    if (pipe(link) == -1) {
        result.status = ERR_LINK_FAIL;
        std::cerr << "error: " << result.status << std::endl;
        return result;
    }

    result.startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());

    void *shmen = mmap(nullptr, sizeof(uint), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);

    if ((pid = fork()) == -1) {
        result.status = ERR_FORK_FAIL;
        std::cerr << "error: " << result.status << std::endl;
        return result;
    }

    // Runs the compiler
    if (pid == 0) {
        dup2(link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);

        if (chdir(directory.c_str())) {
            memcpy(shmen, &SCMErrors::linkFail, sizeof(SCMErrors::linkFail));
            std::cerr << "error: " << result.status << std::endl;
            return result;
        }

        memcpy(shmen, &SCMErrors::execFail, sizeof(SCMErrors::execFail));
        execl(executable.c_str(), executable.c_str(), fileToCompile.c_str(),
              (char *) 0);
        //execl("/bin/ls", "ls", "-l", (char *)0);

        memcpy(shmen, &SCMErrors::execFail, sizeof(SCMErrors::execFail));
        std::cerr << "error: " << result.status << std::endl;
        return result;

    // Gets feedback from the compiler
    } else {
        // Old output might be recycled if the ' = ""' is removed!
        char buffer[4096] = "";
        int status;

        close(link[1]);

        waitpid(-1, &status, 0);

        read(link[0], buffer, sizeof(buffer) - 1);

        result.output = std::string(buffer);
        result.filename = fileToCompile;
        result.status = *static_cast<int *>(shmen);

        munmap(shmen, sizeof(uint));

        result.endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch());
    }
    return result;
}

std::vector<CompileResult> Compiler::CompileDirectory(std::string directory, std::string executable) {
    DIR *dir;
    struct dirent *ent;
    std::vector<CompileResult> results;
    if ((dir = opendir(directory.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string fileName = ent->d_name;

            // Sourcepawn source files have the .sp extension
            if (StrEndsWith(fileName, ".sp")) {
                results.push_back(CompileSingleFile(directory, executable, fileName));
            }
        }

        closedir(dir);
    } else {
        /* could not open directory */
        std::cerr << "Could not open directory" << std::endl;

        return results;
    }

    return results;
}

bool Compiler::StrEndsWith(std::string toSearch, std::string toFind) {
    int startpos = (int) toSearch.size() - (int) toFind.size();

    if (startpos < 0)
        return false;

    return toFind.compare(0, toFind.size(), toSearch, startpos, toSearch.size()) == 0;
}
