#include "test_lib.h"
#include <stdint.h>
#include <stdio.h>

#define Color_Red "\33[31m" 
#define Color_Green "\33[32m" 
#define Color_end "\33[0m" // To flush out prev settings

static uint8_t passed = 0;
static uint8_t failed = 0;

void PASSED(const char* mex){
    printf(Color_Green "%s" Color_end "\n",mex); 
    passed++;
}

void FAILED(const char* mex)
{
    printf(Color_Red "%s" Color_end "\n",mex);
    failed++;
}

void print_SCORE(void)
{
    printf("========================================\n");
    printf("passed %d\n",passed);
    printf("failed %d\n",failed);
    printf("========================================\n");
}
