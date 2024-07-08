#include "canlib.h"
#include <stdio.h>

// Initialize CAN interface
int can_init(const char *ifname) {
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

    return s;
}

// Send a CAN frame
int can_send_frame(int socket, struct can_frame *frame) {
    int nbytes;

    nbytes = write(socket, frame, sizeof(struct can_frame));
    if (nbytes != sizeof(struct can_frame)) {
        perror("Write");
        return -1;
    }

    return 0;
}

// Receive a CAN frame
int can_recv_frame(int socket, struct can_frame *frame) {
    int nbytes;

    nbytes = read(socket, frame, sizeof(*frame));
    if (nbytes != sizeof(struct can_frame)) {
        perror("Read");
        return -1;
    }

    return 0;
}

