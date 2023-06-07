#ifndef PANIC_H
#define PANIC_H

void panic_helper(const char *func, const char *file, const int line, const char * format, ...)
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
#define panic(...)  panic_helper(__func__, __FILE__, __LINE__, __VA_ARGS__)

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

#endif /* end of include guard: PANIC_H */
