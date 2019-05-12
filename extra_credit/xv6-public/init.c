// init: The initial user-level program

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"
#include "bcrypt.h"
#define MAX_CHAR_LEN 100

char *argv[] = { "sh", 0 };
void makePassword(char* entered_password, char* verified_password) {
    printf(0, "Enter password: ");
    gets(entered_password, MAX_CHAR_LEN);
    printf(0, "Re-enter to confirm: ");
    gets(verified_password, MAX_CHAR_LEN);
    }

void enterPassword(char* entered_password) {
    printf(0, "Enter password: ");
    gets(entered_password, MAX_CHAR_LEN);
    }

void log_in(){
    int fd = open("xv6_OS_password", O_RDONLY);
    uchar salt_buffer[BCRYPT_HASHLEN];
    uchar hash_buffer[BCRYPT_HASHLEN];
    read(fd, salt_buffer, BCRYPT_HASHLEN);
    read(fd, hash_buffer, BCRYPT_HASHLEN);
    char pass [MAX_CHAR_LEN];
    printf(0, "Enter password: ");
    gets(pass, MAX_CHAR_LEN);
    int i = 0;
    for (i = 0; i < 3; i++){
        if (bcrypt_checkpass(pass, salt_buffer, hash_buffer) != 0) {
                printf(0, "Invalid attempt, re-enter password: ");
                gets(pass, MAX_CHAR_LEN);
        }else{
            printf(1,"Password correct, logging you in.");
        }
    }
    close(fd);
}

int
main(void)
{
    int pid, wpid;
    
    int password_made = 0;
    char entered_password[MAX_CHAR_LEN];
    char verified_password[MAX_CHAR_LEN];
    int random_salt[10];
    uchar salt[16];
    int i = 0;
    int j = 0;
    int k = 0;
    
    if(open("console", O_RDWR) < 0){
    mknod("console", 1, 1);
    open("console", O_RDWR);
    }
    dup(0);  // stdout
    dup(0);  // stderr

    int fd = open("xv6_OS_password", O_RDWR);
    if (fd < 0) {// if not made
        close(fd);
        fd = open("xv6_OS_password", O_CREATE|O_RDWR); // make one to store
    }
    else {
        password_made = 1; // when it's already made
    }
    while (!password_made) {
        if (fd < 0)
            printf(0, "No password set. Please choose one.\n");
        makePassword(entered_password, verified_password);
        if (strcmp(entered_password, verified_password) == 0) { // verify making password
            printf(0, "Password successfully set. You may now use it to log in.\n");
            for (i = 0; i < 10; i++) {
                random_salt[i] = random();
                }
            for (j = 0; j < 20; j+=5) {
                *((int*)salt+j) = random_salt[k++];
                }
            write(fd, salt, BCRYPT_HASHLEN);
            write(fd, bcrypt(verified_password, salt), BCRYPT_HASHLEN);
            close(fd);
            break; // authentication success
            }
        else {
            printf(0, "Passwords do not match. Try again.\n");
        }
    }
    log_in();
    
    
    
    
    for(;;){
    printf(1, "init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf(1, "init: fork failed\n");
      exit();
    }
    if(pid == 0){
      exec("sh", argv);
      printf(1, "init: exec sh failed\n");
      exit();
    }
    while((wpid=wait()) >= 0 && wpid != pid)
      printf(1, "zombie!\n");
    }
}
