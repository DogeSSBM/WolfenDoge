#ifndef GETINT_H
#define GETINT_H

int strToInt(const char *str)
{
    if(!str)
        panic("Cannot convert NULL str to int!");
    const int len = strlen(str);
    int n = 0;
    for(int i = 0; i < len; i++){
        n *=  10;
        n += *(str+(len-1)-i) - '0';
    }
    return n;
}

uint digits(int n)
{
    int p = 1;
    uint d = 0;
    while(n){
        n += n > 0 ? -n % p : n / p;
        p *= 10;
    }
    return d;
}

//  123
// ""
// 
// char* intToStr(int n, char *str)
// {
//     if(!str)
//         panic("Cannot store int into a NULL str!");
//     const uint l = strlen(str);
//     const uint d = digits(n);
//     for(int i = 0; n; i++){
//
//     }
//     return n;
// }

int shiftR(const int num)
{
    return num / 10;
}

int getR(const int num)
{
    return num % 10;
}

char intToChar(const int num)
{
    if(num > 9 || num < 0)
        return '0';
    else
        return (char)(num + '0');
}

void intToStr(int num, char* str)
{
    for(int i = strlen(str); i > 0 && num > 0; i--){
        str[i-1] = intToChar(getR(num));
        num = shiftR(num);
    }
}

int getInt(void)
{
    char buffer[10];
    memset(buffer, '\0', 10);
    for(int i = 0; i < 9; i++){
        buffer[i] = getchar();
        if(buffer[i] == '\n'){
            buffer[i] = '\0';
            if(i == 0)
                return 0;
            else
                return strToInt(buffer);
        }
    }
    if(getchar()!='\n'){
        printf("\nNumber too large, using [%s] to prevent stack overflow!\n", buffer);
        while(getchar() != '\n');
    }
    return strToInt(buffer);
}

#endif /* end of include guard: GETINT_H */
