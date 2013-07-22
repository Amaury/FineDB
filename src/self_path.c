#include <string.h>
#include "self_path.h"

#ifdef __linux__

# include <limits.h>
# include <stdlib.h>
# include <unistd.h>

const char *get_self_path() {
	static char path[PATH_MAX];
	char *pt;

	if (realpath("/proc/self/exe", path) == NULL)
		return (NULL);
	if ((pt = strrchr(path, '/')) != NULL)
		*pt = '\0';
	return (path);
}

#elif defined _WIN32 || defined _WIN64

# include <stdlib.h>
# include <windows.h>

const char *get_self_path() {
	static TCHAR path[MAX_PATH];

	GetModuleFileName(NULL, path, MAX_PATH);
	if ((pt = strrchr(path, '/')) != NULL)
		*pt = '\0';
	return (path);
}

#elif defined __APPLE__

# include <mach-o/dyld.h>
# include <string.h>
# include <unistd.h>

const char *get_self_path() {
	size_t sz = 0;
	static char *path = NULL;
	char *pt;

	if (path != NULL)
		return (path);
	_NSGetExecutablePath(NULL, &sz);
	path = (char*)malloc(++sz);
	_NSGetExecutablePath(path, &sz);
	if ((pt = strrchr(path, '/')) != NULL)
		*pt = '\0';
	return (path);
}

#endif
