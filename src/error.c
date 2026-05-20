
#include <error.h>
#include <rt.h>

void errorimpl(char *msg, const char *func, const char *file, int line, ...)
{
        va_list args;
        va_start(args, line);
        fprintf(stderr, "error by %s:%d (%s): ", file, line, func);
        vfprintf(stderr, msg, args);
        fprintf(stderr, "\n");
        va_end(args);
        RT_Cleanup();
        abort();
}
