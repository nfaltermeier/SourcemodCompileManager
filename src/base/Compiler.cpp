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
#include <chrono>

#define ERR_LINK_FAIL (1 << 0)
#define ERR_FORK_FAIL (1 << 1)
#define ERR_CHDIR_FAIL (1 << 2)
#define ERR_EXEC_FAIL (1 << 3)
#define ERR_SEARCH_DIR_FAIL (1 << 4)

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

	result.startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());

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
		result.status = 0;
		result.filename = fileToCompile;
		result.endTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
	}
	return result;
}

std::vector<CompileResult> Compiler::CompileDirectory(std::string directory, std::string executable)
{
	DIR *dir;
	struct dirent *ent;
	std::vector<CompileResult> results;
	if ((dir = opendir(directory.c_str())) != NULL) {
	  while ((ent = readdir (dir)) != NULL) {
		  std::string string = ent->d_name;
		  // Sourcepawn source files have the .sp extension
		  if(StrEndsWith(string, ".sp"))
		  {
			  results.push_back(CompileSingleFile(directory, executable, string));
		  }
	  }
	  closedir (dir);
	} else {
	  /* could not open directory */
		std::cerr << "Could not open directory" << std::endl;
	  return results;
	}

	return results;
}

bool Compiler::StrEndsWith(std::string toSearch, std::string toFind)
{
	int startpos = toSearch.size() - toFind.size();
	if(startpos < 0)
		return false;
	return toFind.compare(0, toFind.size(), toSearch, startpos, toSearch.size()) == 0;
}
