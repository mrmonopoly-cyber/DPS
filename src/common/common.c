#include "common.h"

int8_t send_mex_and_wait(const DpsCommon* const restrict self, const DpsCanMessage* const restrict mex)
{
  int8_t err=0;
  err=self->send_f(mex);
  if (self->wait_f)
  {
    self->wait_f();
  }
  return err;
}
