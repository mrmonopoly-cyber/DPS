#include "score_lib/test_lib.h"
#include "linux_board/linux_board.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/cdefs.h>
#include <threads.h>
#include <unistd.h>

int main(void)
{
  if(create_virtual_chip() <0){
    goto end;
  }

  if (virtual_can_manager_init()<0) {
    goto end;
  }

end:
  print_SCORE();
  return 0;
}
