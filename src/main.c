
#include <rt.h>
#include <tok.h>
#include <parse.h>

char *LoadFile(char *Path)
{
        FILE *f = fopen(Path, "rb");
        if (!f)
                return NULL;
        fseek(f, 0, SEEK_END);
        size_t s = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *buf = calloc(1, s + 1);
        if (!buf)
        {
                fclose(f);
                return NULL;
        }

        fread(buf, 1, s, f);
        fclose(f);
        return buf;
}

int main(int argc, char **argv)
{
        if (argc != 2)
                return 1;
        char *buf = LoadFile(argv[1]);
        if (!buf)
                return 2;
        RT_Initialise();
        Execute(buf);
        RT_Cleanup();
        free(buf);
        return 0;
}
