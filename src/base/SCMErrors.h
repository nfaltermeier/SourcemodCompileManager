/*
 * SCMErrors.h
 *
 *  Created on: Mar 29, 2018
 *      Author: nathan
 */

#ifndef SRC_BASE_SCMERRORS_H_
#define SRC_BASE_SCMERRORS_H_

#define ERR_LINK_FAIL (1 << 0)
#define ERR_FORK_FAIL (1 << 1)
#define ERR_CHDIR_FAIL (1 << 2)
#define ERR_EXEC_FAIL (1 << 3)
#define ERR_SEARCH_DIR_FAIL (1 << 4)

namespace SCMErrors
{
	extern int linkFail;
	extern int forkFail;
	extern int chdirFail;
	extern int execFail;
	extern int searchDirFail;
}



#endif /* SRC_BASE_SCMERRORS_H_ */
