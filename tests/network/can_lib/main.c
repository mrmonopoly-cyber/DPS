#include "canlib.h"
#include <stdio.h>

int main() {
    int s;
    struct can_frame frame;

    // Initialize CAN interface
    s = can_init("test_can_dps");
    if (s < 0) {
        return 1;
    }

    // Prepare a CAN frame to send
    frame.can_id = 0x123;
    frame.can_dlc = 8;
    for (int i = 0; i < 8; i++) {
        frame.data[i] = i;
    }

    // Send the CAN frame
    if (can_send_frame(s, &frame) < 0) {
        close(s);
        return 1;
    }

    printf("wait: ");
    char a;
    scanf("%c",&a);
    printf("ok\n");

    // Receive a CAN frame
    if (can_recv_frame(s, &frame) < 0) {
        close(s);
        return 1;
    }

    // Print the received frame
    printf("Received CAN frame with ID: 0x%X, Data: ", frame.can_id);
    for (int i = 0; i < frame.can_dlc; i++) {
        printf("%02X ", frame.data[i]);
    }
    printf("\n");

    // Close the socket
    close(s);

    return 0;
}

