#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include "tunnel.h"
#include "icmp_cover.h"
#include "defs.h"

int main() {
    fd_set read_fds = {0};
    int err = EXIT_SUCCESS;
    int num_read = 0;
    char buffer[BUFF_SIZE];
    char* new_buffer = NULL;
    int new_size = 0;

    int fd = tun_alloc(TUN_NAME);

    if (fd == -1) {
        return EXIT_FAILURE;
    }

    printf("%s", "Interface tun33 created successfully\n");

    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(fd, &read_fds);

        err = select(fd + 1, &read_fds, NULL, NULL, NULL);
        
        if (err <= 0) {
            break;
        }

        num_read = tun_read(fd, buffer, BUFF_SIZE);

        if (num_read == -1) {
            break;
        }

        new_size = build_icmp_cover(buffer + 4, &new_buffer, num_read, false);  

        if (new_size == -1) {
            break;
        }

        err = send_icmp_covered_packet(new_buffer, new_size, SERVER_ADDR);
        printf("packet sent\n");
        if (err == -1) {
            break;
        }

        free(new_buffer);     
    }

 
    tun_close(fd);
    printf("%s", "Interface tun33 closed\n");

    return EXIT_SUCCESS;
}

