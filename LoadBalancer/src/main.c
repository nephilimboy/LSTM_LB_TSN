#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>


#define TARGETFILE "targets.txt"
#define MAXPAYLOADSIZE 65507
#define MAXTARGET 128
#define WORDSIZE  256
#define LBPORT 8080
#define HOSTIP "127.0.0.1"

char *convert_stringIP_to_inetIP(char *hostname)
{
    struct hostent *hnet = gethostbyname(hostname);
    if (hnet == NULL) {
        printf("Host \"%s\" not found\n", hostname);
        exit(1);
    }
    struct in_addr **addr_IPlist = (struct in_addr **)hnet->h_addr_list;
    return inet_ntoa(*addr_IPlist[0]);
}

int main(void)
{
    char line[MAXTARGET][WORDSIZE];
    char fname[20];
    FILE *fptr = NULL;
    unsigned int i = 0;

    int total_target_hosts = 0;
    // init value 99
    int total_target_hosts_previous_iteration = 99;
    unsigned int LBport = LBPORT;
    char *LBip = HOSTIP;

    fptr = fopen(TARGETFILE, "r");
    while(fgets(line[i], WORDSIZE, fptr))
    {
        line[i][strlen(line[i]) - 1] = '\0';
        i++;
    }
    fclose(fptr);
    total_target_hosts = i;
    total_target_hosts_previous_iteration = total_target_hosts;
    printf("\n List of targets: \n");

    char **targets_ip = malloc(MAXTARGET * sizeof(char*));
    for(i = 0; i < total_target_hosts; i++) {
        targets_ip[i] = strdup(line[i]);
        printf(" %s\n", line[i]);
    }

    printf("\nHost %s:%d\n", LBip, LBport);

    // Creating a socket and its address
    int lb_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in lb_host_addr;

    lb_host_addr.sin_family = AF_INET;
    lb_host_addr.sin_port = htons(LBport);
    lb_host_addr.sin_addr.s_addr = inet_addr(LBip);
    bind(lb_socket,(struct sockaddr*)&lb_host_addr,sizeof(lb_host_addr));

    // Packet load balancing round-robin
    char pkt_message[MAXPAYLOADSIZE];
    unsigned int pkt_length;

    for (i = 0; i < total_target_hosts; i++) {
            targets_ip[i] = convert_stringIP_to_inetIP(targets_ip[i]);
        }

    while(1) {
        i = 0;
        fptr = fopen(TARGETFILE, "r");
        while(fgets(line[i], WORDSIZE, fptr))
        {
            line[i][strlen(line[i]) - 1] = '\0';
            i++;
        }
        fclose(fptr);
        total_target_hosts = i;
        if(total_target_hosts_previous_iteration != total_target_hosts) {
            total_target_hosts_previous_iteration = total_target_hosts;
            free(targets_ip);
            *targets_ip = malloc(MAXTARGET * sizeof(char*));
            for(i = 0; i < total_target_hosts; i++) {
                targets_ip[i] = strdup(line[i]);
                printf(" %s\n", line[i]);
            }
        }

        for(i = 0; i < total_target_hosts; i++) {

            unsigned int orig_senderIP_length;
            struct sockaddr_in orig_senderIP;

            *(&pkt_length) = recvfrom(
                lb_socket, pkt_message,
                MAXPAYLOADSIZE,
                0,
                (struct sockaddr *) &orig_senderIP,
                &orig_senderIP_length
            );

            // Make target host address
            struct sockaddr_in targetHostIP;
            targetHostIP.sin_family = AF_INET;
            targetHostIP.sin_port = htons(LBport);
            targetHostIP.sin_addr.s_addr = inet_addr(targets_ip[i]);

            sendto(lb_socket, pkt_message, pkt_length, 0, (struct sockaddr*)&targetHostIP, sizeof(targetHostIP));

        }

    }

    return 0;
}

