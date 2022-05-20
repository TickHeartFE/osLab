#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main() {
    char input_str[100];
    scanf("%s", input_str);
    const char s[2] = ",";
    // 获取第一个字符串
    char* token = strtok(input_str, s);
    // 继续获取别的字符串
    while(token != NULL) {
        printf("%s ", token);
        token = strtok(NULL, s);
    }
    return 0;
}

