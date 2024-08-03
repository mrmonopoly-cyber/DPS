#ifndef __PDS_COMMON_FUNC__
#define __PDS_COMMON_FUNC__

typedef struct GenericFun {
    void* (*generic_fun) (void* args);
    char fun_name[];
}GenericFun;


#endif // !__PDS_COMMON_FUNC__
