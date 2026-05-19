
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

#endif
