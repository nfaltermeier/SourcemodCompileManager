/*
 * Compiler.cpp
 *
 *  Created on: Oct 24, 2017
 *      Author: nsfaltermeier
 */

#include "Compiler.h"
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <dirent.h>

#define ERR_LINK_FAIL (1 << 0)
#define ERR_FORK_FAIL (1 << 1)
#define ERR_CHDIR_FAIL (1 << 2)
#define ERR_EXEC_FAIL (1 << 3)

Compiler::Compiler() {
	// TODO Auto-generated constructor stub

}

Compiler::~Compiler() {
	// TODO Auto-generated destructor stub
}

CompileResult Compiler::CompileSingleFile(std::string directory,
		std::string executable, std::string fileToCompile)
{
	int link[2];
	pid_t pid;
	char foo[4096];
	CompileResult result;

	if (pipe(link) == -1) {
		result.status = ERR_LINK_FAIL;
		return result;
	}

	if ((pid = fork()) == -1) {
		result.status = ERR_FORK_FAIL;
		return result;
	}

	if (pid == 0) {

		dup2(link[1], STDOUT_FILENO);
		close(link[0]);
		close(link[1]);
		if (chdir(directory.c_str())) {
			std::cerr << errno << std::endl;
			result.status = ERR_CHDIR_FAIL;
			return result;
		}
		execl(executable.c_str(), executable.c_str(), fileToCompile.c_str(),
				(char *) 0);
		//execl("/bin/ls", "ls", "-l", (char *)0);

		result.status = ERR_EXEC_FAIL;
		return result;

	} else {

		close(link[1]);
		int status;
		waitpid(-1, &status, 0);
		read(link[0], foo, sizeof(foo));
		result.output = std::string(foo);
	}
	return result;
	std::cout << result.output << std::endl;
}

std::vector<CompileResult> Compiler::CompileDirectory(std::string directory, std::string executable)
{
	DIR *dir;
	struct dirent *ent;
	std::vector<CompileResult> results;
	if ((dir = opendir(directory.c_str())) != NULL) {
	  /* print all the files and directories within directory */
	  while ((ent = readdir (dir)) != NULL) {
	    std::cout << ent->d_name << std::endl;
	  }
	  closedir (dir);
	} else {
	  /* could not open directory */
		std::cerr << "Could not open directory" << std::endl;
	  return results;
	}

	return results;
}
