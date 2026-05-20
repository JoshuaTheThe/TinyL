
#ifndef BUILTIN_H
#define BUILTIN_H

// Builtin functions

#define ADD_BUILTIN(name, func)                                                            \
        do                                                                                 \
        {                                                                                  \
                TOKEN tok = {.Kind = TOKEN_IDENTIFIER};                                    \
                size_t len = strlen(name);                                                 \
                tok.Number = len;                                                          \
                memcpy(tok.Identifier, name, len + 1);                                     \
                RT_CreateVariable(tok, (VALUE){.Type = TYPE_BUILTIN, .as.builtin = func}); \
        } while (0)

void Builtin_AddBuiltinFunctions(void);

#define SETUP(c)              int __rel = c;
#define ARGUMENT(nam)         VALUE nam = RT_Rel(-(__rel-- - 1));
#define INTARGUMENT(nam)      VALUE nam = RT_Rel(-(__rel-- - 1)); if (nam.Type != TYPE_INT) {error("non integer argument given when expected integer");}
#define ARRARGUMENT(nam)      VALUE nam = RT_Rel(-(__rel-- - 1)); if (nam.Type != TYPE_ARRAY) {error("non array argument given when expected array");}

#endif
