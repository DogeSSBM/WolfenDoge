#ifndef DOGELIB_PANIC_H
#define DOGELIB_PANIC_H

void panicHelper(const char *func, const char *file, const int line, const char * format, ...)
{
    fprintf(stderr, "Panic -\n");
    fprintf(stderr, "\tFile: %s\n", file);
    fprintf(stderr, "\tLine: %i\n", line);
    fprintf(stderr, "\tFunc: %s\n", func);
    fprintf(stderr, "\tMessage: \"");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\"\n");
    exit(EXIT_FAILURE);
}
#define panic(...)  panicHelper(__func__, __FILE__, __LINE__, __VA_ARGS__)

void notNullHelper(const char *func, const char *file, const int line, void *ptr)
{
    if(ptr == NULL){
        fprintf(stderr, "Unexpected NULL -\n");
        fprintf(stderr, "\tFile: %s\n", file);
        fprintf(stderr, "\tLine: %i\n", line);
        fprintf(stderr, "\tFunc: %s\n", func);
        exit(EXIT_FAILURE);
    }
}
#define notNull(ptr)  notNullHelper(__func__, __FILE__, __LINE__, ptr)

void assertExprHelper(const char *func, const char *file, const int line, const bool val, const char *expr)
{
    if(!val){
        fprintf(stderr, "Error unexpected expression result -\n");
        fprintf(stderr, "\tFile: %s\n", file);
        fprintf(stderr, "\tLine: %i\n", line);
        fprintf(stderr, "\tFunc: %s\n", func);
        fprintf(stderr, "\tExpr: %s\n", expr);
        exit(EXIT_FAILURE);
    }
}
#define assertExpr(expr)  assertExprHelper(__func__, __FILE__, __LINE__, expr, #expr)

void assertLogExprHelper(const char *func, const char *file, const int line, const bool val, const char *expr)
{
    if(val){
        printf("log(%s, %s, %i): %s\n", func, file, line, expr);
        return;
    }
    fprintf(stderr, "assertLogExpr failed -\n");
    fprintf(stderr, "\tFile: %s\n", file);
    fprintf(stderr, "\tLine: %i\n", line);
    fprintf(stderr, "\tFunc: %s\n", func);
    fprintf(stderr, "\tFalse expression: %s\n", expr);
    exit(EXIT_FAILURE);
}
#define assertLogExpr(expr) assertLogExprHelper(__func__, __FILE__, __LINE__, expr, #expr)

void logMsgHelper(const char *func, const char *file, const int line, const char * format, const char *exprs,...)
{
    fprintf(stderr, "Log -\n");
    fprintf(stderr, "\tFile: %s\n", file);
    fprintf(stderr, "\tLine: %i\n", line);
    fprintf(stderr, "\tFunc: %s\n", func);
    fprintf(stderr, "\tMessage: \"");
    va_list args;
    va_start(args, exprs);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\"\n");
    fprintf(stderr, "\tExprs: \"%s\"\n", exprs);
}
#define logMsg(format, ...)  logMsgHelper(__func__, __FILE__, __LINE__, format, #__VA_ARGS__, __VA_ARGS__)

void logExprHelper(const char * format, const char *exprs,...)
{
    fprintf(stderr, "{%s} -> {", exprs);
    va_list args;
    va_start(args, exprs);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "}\n");
}
#define logExpr(format, ...)  logExprHelper(format, #__VA_ARGS__, __VA_ARGS__)

void assertExprMsgHelper(const char *func, const char *file, const int line, const bool val, const char *expr, const char *format, ...)
{
    if(!val){
        fprintf(stderr, "Error unexpected expression result -\n");
        fprintf(stderr, "\tFile: %s\n", file);
        fprintf(stderr, "\tLine: %i\n", line);
        fprintf(stderr, "\tFunc: %s\n", func);
        fprintf(stderr, "\tExpr: %s\n", expr);
        fprintf(stderr, "\tMessage: \"");
        va_list args;
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fprintf(stderr, "\"\n");
        exit(EXIT_FAILURE);
    }
}
#define assertExprMsg(expr, ...)  assertExprMsgHelper(__func__, __FILE__, __LINE__, expr, #expr, __VA_ARGS__)

void assertLogExprMsgHelper(const char *func, const char *file, const int line, const bool val, const char *expr, const char *format, ...)
{
    if(val){
        printf("log(%s, %s, %i): %s\n", func, file, line, expr);
        return;
    }
    fprintf(stderr, "assertLogExpr failed -\n");
    fprintf(stderr, "\tFile: %s\n", file);
    fprintf(stderr, "\tLine: %i\n", line);
    fprintf(stderr, "\tFunc: %s\n", func);
    fprintf(stderr, "\tFalse expression: %s\n", expr);
    fprintf(stderr, "\tMessage: \"");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\"\n");
    exit(EXIT_FAILURE);
}
#define assertLogExprMsg(expr, ...) assertLogExprMsgHelper(__func__, __FILE__, __LINE__, expr, #expr, __VA_ARGS__)

#endif /* end of include guard: DOGELIB_PANIC_H */
