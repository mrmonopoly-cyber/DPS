#include "../../dps_slave.h"
#include <stdint.h>
#include <stdio.h>

uint8_t send(CanMessage* mex){
    return 0;
}

uint8_t u8 =0;
uint16_t u16 = 0;
uint32_t u32= 0;
uint64_t u64= 0;

int8_t s8 =0;
int16_t s16 = 0;
int32_t s32= 0;
int64_t s64= 0;

float fdata= 0;
double ddata= 0;

int check_monitor()
{
    {
        VariableInfoName name = {
            .full_data.name = "u8",
        };
        if (dps_monitor_var_uint8_t(&name, &u8)){
            perror("error saving var uint8_t");
            return -2;
        }
        printf("check ok save var uint8_t");
    }

    {
        VariableInfoName name = {
            .full_data.name = "u16",
        };
        if (dps_monitor_var_uint16_t(&name, &u16)){
            perror("error saving var uint16_t");
            return -3;
        }
        printf("check ok save var uint16_t");
    }

    {
        VariableInfoName name = {
            .full_data.name = "u32",
        };
        if (dps_monitor_var_uint32_t(&name, &u32)){
            perror("error saving var uint32_t");
            return -3;
        }
        printf("check ok save var uint32_t");
    }

    {
        VariableInfoName name = {
            .full_data.name = "u64",
        };
        if (dps_monitor_var_uint64_t(&name, &u64)){
            perror("error saving var uint64_t");
            return -3;
        }
        printf("check ok save var uint64_t");
    }

    {
        VariableInfoName name = {
            .full_data.name = "s8",
        };
        if (dps_monitor_var_int8_t(&name, &s8)){
            perror("error saving var int8_t");
            return -2;
        }
        printf("check ok save var int8_t");
    }

    {
        VariableInfoName name = {
            .full_data.name = "s16",
        };
        if (dps_monitor_var_int16_t(&name, &s16)){
            perror("error saving var int16_t");
            return -3;
        }
        printf("check ok save var int16_t");
    }

    {
        VariableInfoName name = {
            .full_data.name = "s32",
        };
        if (dps_monitor_var_int32_t(&name, &s32)){
            perror("error saving var int32_t");
            return -3;
        }
        printf("check ok save var int32_t");
    }

    {
        VariableInfoName name = {
            .full_data.name = "s64",
        };
        if (dps_monitor_var_int64_t(&name, &s64)){
            perror("error saving var int64_t");
            return -3;
        }
        printf("check ok save var int64_t");
    }
    return 0;
}

int main(void)
{
    BoardName board_name = {
        .full_data.name = "TSLAVE",
    };

    if (dps_init(send,&board_name)){
        perror("failed init of slave");
        return -1;
    }

    if (check_monitor()){
        perror("failed saving vars");
        return -2;
    }

    return 0;
}
