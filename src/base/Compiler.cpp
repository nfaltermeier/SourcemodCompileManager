#include "Compiler.h"
#include <unistd.h>
#include <sys/wait.h>
#include <dirent.h>
#include <chrono>
#include <sys/mman.h>
#include "SCMErrors.h"

CompileResult Compiler::CompileSingleFile(const std::string& directory, const std::string& executable,
                                          const std::string& fileToCompile, const std::string& compilerArgs,
                                          const std::string& outputDirectory) {
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

    auto* shmen = (uint*) mmap(nullptr, sizeof(uint), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);

    if ((pid = fork()) == -1) {
        result.status = ERR_FORK_FAIL;
        std::cerr << "error: " << result.status << std::endl;
        return result;
    }

    std::string outputLocation;
    if (!outputDirectory.empty()) {
        outputLocation = "-o" + outputDirectory + fileToCompile
    }

    // Runs the compiler
    if (pid == 0) {
        dup2(link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);

        if (chdir(directory.c_str())) {
            *shmen = ERR_LINK_FAIL;
            std::cerr << "error: " << result.status << std::endl;
            return result;
        }

        execl(executable.c_str(), executable.c_str(), fileToCompile.c_str(), compilerArgs.c_str(),
              nullptr);

        *shmen = ERR_EXEC_FAIL;
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
        result.status = *shmen;

        munmap(shmen, sizeof(uint));

        result.endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch());
    }
    return result;
}

std::vector<CompileResult> Compiler::CompileDirectory(const std::string& directory, const std::string& executable,
                                                      const std::string& compilerArgs, const std::string& outputDirectory) {
    DIR *dir;
    struct dirent *ent;
    std::vector<CompileResult> results;
    if ((dir = opendir(directory.c_str())) != nullptr) {
        while ((ent = readdir(dir)) != nullptr) {
            std::string fileName = ent->d_name;

            // Sourcepawn source files have the .sp extension
            if (StrEndsWith(fileName, ".sp")) {
                results.push_back(CompileSingleFile(directory, executable, fileName, compilerArgs, outputDirectory));
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

bool Compiler::StrEndsWith(const std::string& toSearch, const std::string& toFind) {
    size_t toSearchSize = toSearch.size();
    size_t toFindSize = toFind.size();

    // If the string to find is bigger than the string to find it in
    if (toFindSize > toSearchSize)
        return false;

    size_t startpos = toSearchSize - toFindSize;
    return toFind.compare(0, toFindSize, toSearch, startpos, toSearchSize) == 0;
}
