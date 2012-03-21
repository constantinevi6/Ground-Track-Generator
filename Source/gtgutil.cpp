#include "gtgutil.h"

#include <stdio.h>
#include <stdlib.h>

/* Print an error message to stderr and exit with failure status. */
void FailDetail(const char *file, int line, const char *errorString, ...) {
	va_list arglist;
	va_start(arglist, errorString);
	fprintf(stderr, "%s:%s:%d: ", _GTG_PROGRAM_, file, line);
	vfprintf(stderr, errorString, arglist);
	va_end(arglist);
	exit(EXIT_FAILURE);
}

/* Print a diagnostic message. We could rewrite these to a log file. */
void Note(const char *noteString, ...) {
	va_list arglist;
	if (cfg.verbose) {
		va_start(arglist, noteString);
		vprintf(noteString, arglist);
		va_end(arglist);
	}
}

void ShowVersion(void)
{
	printf("%s %s\n", _GTG_NAME_, _GTG_VERSION_);
	exit(EXIT_SUCCESS);
}

void ShowHelp(void)
{
	printf("%s %s\n", _GTG_NAME_, _GTG_VERSION_);
	printf("usage: %s [OPTIONS] [OUTPUT]\n", _GTG_PROGRAM_);
	printf("Options:\n");
	printf("\t--unit days | hours | minutes | seconds\n");
	printf("\t\tDefault: minutes\n");
	printf("\t--interval N\n");
	printf("\t\tDefault: 1.0\n");
	printf("\t--steps N\n");
	printf("\t\tDefault: 100\n");
	printf("\t--start now | epoch | YYYY-MM-DD HH:MM:SS.SSSSSS UTC | S\n");
	printf("\t\tDefault: epoch (TLE reference date)\n");
	printf("\t--end now | epoch | YYYY-MM-DD HH:MM:SS.SSSSSS UTC | S\n");
	printf("\t\tIf specified, overrides --steps.\n");
	printf("\t--format point | line\n");
	printf("\t\tDefault: point\n");
	printf("\t--tle TEXT | --input PATH\n");
	printf("\t\tTLE read from TEXT value, PATH file, or otherwise stdin.\n");
	printf("\t--output BASEPATH\n");
	printf("\t\tIf not specified, BASEPATH be provided as OUTPUT argument.\n");
	printf("Web page: <https://github.com/anoved/Ground-Track-Generator>\n");
	exit(EXIT_SUCCESS);
}

