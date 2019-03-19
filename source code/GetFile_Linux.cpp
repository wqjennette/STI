#include <stdio.h>

void GetFile(char* buffer, size_t bufferlen)
{
    printf("Enter a filename (e.g. small.ogv): ");
    scanf("%s", buffer);
}