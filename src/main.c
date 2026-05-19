
#include <rt.h>
#include <tok.h>
#include <parse.h>

int64_t execute(char *s)
{
        char *p = s;
        int64_t res = 0;
        while (TK_Peek(&p).Kind != TOKEN_EOF)
        {
                parse_expr(&p);
        }

        return res;
}

int main(int argc, char **argv)
{
        if (argc != 2)
                return 1;
        FILE *f = fopen(argv[1], "rb");
        if (!f)
                return 1;
        fseek(f, 0, SEEK_END);
        size_t s = ftell(f);
        fseek(f, 0, SEEK_SET);
        char *buf = calloc(1, s + 1);
        fread(buf, 1, s, f);
        RT_EnterScope(true);
        int res = execute(buf);
        free(buf);
        fclose(f);
        RT_Cleanup();
        return res;
}
