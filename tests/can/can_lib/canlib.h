#ifndef CANLIB_H
#define CANLIB_H

#include <linux/can.h>
#include <stdint.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

// Function prototypes
int can_init_full(const char* const ifname, const uint16_t filter_id, const uint16_t mask_id);
#define can_init(ifname) can_init_full(ifname, 0, 0);
int can_send_frame(int socket, struct can_frame *frame);
int can_recv_frame(int socket, struct can_frame *frame);

#endif // CANLIB_H

