#include "canlib.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

// Initialize CAN interface
int can_init_full(const char* const ifname, const uint16_t filter_id, const uint16_t mask_id)
{
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;

    // Create a socket
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Socket");
        return -1;
    }

    // Specify the CAN interface
    strcpy(ifr.ifr_name, ifname);
    if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
        perror("Ioctl");
        return -1;
    }

    // Bind the socket to the CAN interface
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind");
        return -1;
    }
    
    struct can_filter filter={
      .can_id = filter_id,
      .can_mask = mask_id,
    };

    if(setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter))<0)
    {
      perror("Setsocket error");
      return -1;
    }

    return s;
}

// Send a CAN frame
int can_send_frame(int socket, struct can_frame *frame) {
    int nbytes;

    nbytes = write(socket, frame, sizeof(*frame));
    if (nbytes != sizeof(struct can_frame)) {
        perror("Write");
        return -1;
    }

    return 0;
}

// Receive a CAN frame
int can_recv_frame(int socket, struct can_frame *frame) {
  int nbytes;

  fcntl(socket, F_SETFL, fcntl(socket, F_GETFL,0) | O_NONBLOCK);

  nbytes = read(socket, frame, sizeof(*frame));
  return nbytes;
}

