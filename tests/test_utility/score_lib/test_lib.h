#ifndef __DPS_TEST_LIB__
#define __DPS_TEST_LIB__

void PASSED(const char *mex);
void FAILED(const char *mex);

#define TEST_EXPR(expr, description)\
  if(expr){\
    FAILED(description);\
  }else{\
    PASSED(description);\
  }

void print_SCORE(void);

#endif // !__DPS_TEST_LIB__
