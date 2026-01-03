#include "test_utility/test_utility.h"
#include "src/master/dps_master.h"
#include "src/slave/dps_slave.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

typedef enum{
  Board_1 = 1,
  Board_2 = 2,
  Board_3 = 3,
}BoardIndex;

NEW_BOARD(
    uint8_t u8_th;
    int32_t s32_user;
    )board1 ={.u8_th = 12, .s32_user = 55};

NEW_BOARD(
    float f_stw;
    )board2 ={.f_stw = 19.2f};

NEW_BOARD(
    uint64_t u64;
    int64_t i64;
    double f64;
    )board3 ={.u64 = 12, .i64= -200, .f64 = -3.14};

static inline int _check_num_vars(const VarListInfo* const vars, const BoardIndex board, const int num_vars)
{
  const int same= vars->var_num == num_vars;
  TEST_EXPR(!same, "board valid num vars:");
  printf("board: %d, given: %d, expected: %d\n",board, vars->var_num, num_vars);

  return same;
}

static inline int _check_name_var(const VarRecord* const var,
    const char name[VAR_NAME_LENGTH])
{
  const int same= !strncmp(var->name, name, VAR_NAME_LENGTH);
  TEST_EXPR(!same, "var valid name:");
  printf("given: %s, expected: %s\n", var->name, name);

  return same;
}

static inline int _check_type_var(const VarRecord* const var,
    const enum DATA_GENERIC_TYPE d_type, const int d_size)
{
  const int given_size = (1<<var->size);
  const int same = d_type == var->type &&(1<< d_size) == given_size;

  TEST_EXPR(!same, "valid var type:");
  printf("type: given: %d, expected: %d\n", var->type, d_type);
  printf("size: given: %d, expected: %d\n", given_size, 1<< d_size);

  return same;
}

static void _populate_var_record(VarRecord* rec,
    const char name[VAR_NAME_LENGTH],
    const enum DATA_GENERIC_TYPE type,
    const uint8_t size,
    const void* raw_value)
{
  strncpy(rec->name, name, VAR_NAME_LENGTH);
  rec->type = type;
  rec->size = size;
  memcpy(&rec->v_u64, raw_value, 1<<size);
}

static int _check_board_var(const VarListInfo* const vars, const BoardIndex board_num)
{
  int err=0;
  int num_vars = 0;
  int cursor=0;
  const int max_vars = 10;

  VarRecord vars_info[max_vars];

  memset(vars_info, 0, sizeof(vars_info));

  switch (board_num)
  {
    case Board_1:
      num_vars = 2;
      _populate_var_record(&vars_info[cursor++], "u8_th", DATA_UNSIGNED, 0, &board1.u8_th);
      _populate_var_record(&vars_info[cursor++], "s32_u", DATA_SIGNED, 2, &board1.s32_user);
      break;
    case Board_2:
      num_vars = 1;
      _populate_var_record(&vars_info[cursor++], "vf_st", DATA_FLOATED, 2, &board2.f_stw);
      break;
    case Board_3:
      num_vars = 3;
      _populate_var_record(&vars_info[cursor++], "u64th", DATA_UNSIGNED, 3, &board3.u64);
      _populate_var_record(&vars_info[cursor++], "i64th", DATA_SIGNED, 3, &board3.i64);
      _populate_var_record(&vars_info[cursor++], "f64th", DATA_FLOATED, 3, &board3.f64);
      break;
    default:
      return -4;
  }

  if(_check_num_vars(vars, board_num, num_vars)<0){
    err=-1;
    goto end;
  }
  for (int i=0; i<vars->var_num; i++){
    const VarRecord* const p_rec = &vars_info[i];
    const VarRecord* const p_ori = &vars->vars[i];
    #define CHECK_AND_SET_ERR(exp) if(exp) err=-2;

    printf("checking var: %d of board: %d\n", i, board_num);
    CHECK_AND_SET_ERR(!_check_name_var(p_ori, p_rec->name));
    CHECK_AND_SET_ERR(!_check_type_var(p_ori, p_rec->type, p_rec->size));
  }

end:
  return err;
}

int main(void)
{
  MasterBoard_t master = {0};

  const uint16_t master_id = 12;
  const uint16_t slaves_id= 22;

  #define fast_dps_slave_init(board, name, id)\
    dps_slave_init(&board.core.m_dps_slave, can_send_test, wait_f, name, id, master_id, slaves_id);

  fast_dps_slave_init(board1, "2_v_b1", Board_1);
  fast_dps_slave_init(board2, "1_v_b1", Board_2);
  fast_dps_slave_init(board3, "3_v_b1", Board_3);

  dps_master_init(&master.m_dps_master, master_id, slaves_id, can_send_test, wait_f);

  start_board(&board1.core);
  start_board(&board2.core);
  start_board(&board3.core);
  start_master_board(&master);


  //BOARD 1
  TEST_EXPR(
      dps_monitor_primitive_var(&board1.core.m_dps_slave, DPS_TYPES_UINT8_T, &board1.u8_th, NULL, "u8_th")<0,
      "board1 monitor u8_th");

  TEST_EXPR(
      dps_monitor_primitive_var(&board1.core.m_dps_slave, DPS_TYPES_INT32_T, &board1.s32_user, NULL, "s32_u")<0,
      "board1 monitor s32_user");

  //BOARD 2
  TEST_EXPR(
      dps_monitor_primitive_var(&board2.core.m_dps_slave, DPS_TYPES_FLOAT_T, &board2.f_stw, NULL, "vf_st")<0,
      "board2 monitor vf_stw");

  //BOARD 3
  TEST_EXPR(dps_monitor_primitive_var(&board3.core.m_dps_slave, DPS_TYPES_UINT64_T, &board3.u64, NULL, "u64th")<0,
      "board3 monitor u64t");

  TEST_EXPR(dps_monitor_primitive_var(&board3.core.m_dps_slave, DPS_TYPES_INT64_T, &board3.i64, NULL, "i64th")<0,
      "board3 monitor i64t");

  TEST_EXPR(dps_monitor_primitive_var(&board3.core.m_dps_slave, DPS_TYPES_DOUBLE_T, &board3.f64, NULL, "f64th")<0,
      "board3 monitor f64t");


  if(dps_master_new_connection(&master.m_dps_master)<0)
  {
    FAILED("master could not connect");
    return -1;
  }

  sleep(2);

  BoardListInfo* boards = dps_master_list_board(&master.m_dps_master);
  if (boards && boards->board_num == 3)
  {
    for (uint8_t i=0; i<boards->board_num; i++)
    {
      dps_master_request_info_board(&master.m_dps_master, boards->boards[i].id, REQ_VAR);
    }
    sleep(5);
    for (uint8_t i=0; i<boards->board_num; i++)
    {
      VarListInfo* vars = dps_master_list_vars(&master.m_dps_master, boards->boards[i].id);
      if (vars)
      {
        if (_check_board_var(vars, boards->boards[i].id)<0)
        {
          FAILED("valid meatada for board: ");
          printf("%d\n",i);
        }
        free(vars);
        vars=NULL;
      }
    }
  }

  //INFO: cleaning
  if (boards)
  {
    free(boards);
  }

  stop_board(&board1.core);
  stop_board(&board2.core);
  stop_board(&board3.core);
  stop_master_board(&master);

  print_SCORE();
  return 0;
}
