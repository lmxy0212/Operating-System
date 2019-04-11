#include "types.h"
#include "stat.h"
#include "user.h"
#define max_size 724

char buf[max_size];

void
uniq(int fd, char *name, int occurance,int print_dup,int ignore_case)
{
    int i, n,j,r,f;
    int l, w, c;
    
    char prevLine[max_size], thisLine[max_size];
    char prepre[max_size];
    l = w = c = j = r = f = 0;
    
    int count_dup;
    count_dup = 1;
    
//    printf(1,"(%d, %d, %d) \n",occurance,print_dup,ignore_case);
//    int ignore_case_cmp(char thisLine[],char prevLine[])
//    {
//        char s1[max_size],s2[max_size];
//        strcpy(s1,thisLine);
//        strcpy(s2,prevLine);
//
//        for(j=0; j<sizeof(thisLine); j++){
//            if((s1[j]>='A' && s1[j]<='Z')) s1[j] += 32;
//            if((s2[j]>='A' && s2[j]<='Z')) s2[j] += 32;
//            //            printf(1,"lower: %s",s1);
//            //            printf(1,"lower: %s \n",s2);
//            if(s1[j] == '\n' || s2[j] == '\n') break;
//        }
//        return strcmp(s1,s2);
//    }
    
    int ignore_case_cmp(const char* this,const char* prev){
        const char* s1 = prev;
        const char* s2 = this;
        char c1, c2;
        while(*s1 != '\0'){
            if(*s1 <= 'Z' && *s1 >= 'A'){
                c1 = *s1 + 32;
            }else{
                c1 = *s1;
            }
            if(*s2 <= 'Z' && *s2 >= 'A'){
                c2 = *s2 + 32;
            }else{
                c2 = *s2;
            }
            if((c1-c2) != 0)return 1;
            s1++;
            s2++;
        }
        if(*s2 == '\0') return 0;
        return 1;
    }

    
    void print_uniq(char temp[],int f){
        if(occurance){ // if count occurance
            printf(1,"%d %s\n",count_dup,temp);
        }else{
            if(print_dup){
//                printf(1,"count dup: %d ",count_dup);
                if(count_dup>1){
                    printf(1,"%s\n",temp);
                }
            }else
                printf(1,"%s\n",temp);
        }
        if(f){
            strcpy(temp,thisLine);
            memset(thisLine, 0, max_size);
        }
    }
    
    char *prev, *this,*prep;
    prev = prevLine;
    this = thisLine;
    prep = prepre;
    
    while((n = read(fd, buf, sizeof(buf))) > 0){
        for(i=0; i<n; i++){
            if(buf[i] == '\n'){  //end of the line
                if(l != 0){//if not the first line, compare w/ prev
                    memset(prepre, 0, max_size);
                    strcpy(prepre,prevLine);
                    
                    if(ignore_case){ //ignore case
                        if(ignore_case_cmp(this,prev) != 0){ // if not dup
                            //                            printf(1,"ignore_case: ");
                            print_uniq(prevLine,1);
                            count_dup = 1;
                        }else{ //  dup
                            count_dup++;
                        }
                    }else{
                        if(strcmp(thisLine,prevLine) != 0){
                            print_uniq(prevLine,1);
                            count_dup = 1;
                        }else{
                            count_dup++;
                        }
                    }
                }else{
                    strcpy(prevLine,thisLine);
                }
                l++;
                c = 0;
            }else{
                thisLine[c] = buf[i];//get this line as a str
                c++;//count #char in this line
            }
        }
    }
//    printf(1,"%s,%s, %s,\n" ,prepre,prevLine,thisLine);
    if(ignore_case){
        if(ignore_case_cmp(this,prev) != 0){ //thisline != preLIne
//            printf(1,"--->%d", ignore_case_cmp(thisLine,prevLine));
            print_uniq(prevLine,0);
            count_dup = 1;
            if(strcmp(thisLine,"") != 0)
                print_uniq(thisLine,0);
        }else{  //thisline == preline
            if(ignore_case_cmp(this,prep) != 0){
//                    print_uniq(prepre,0);
                count_dup = 2;
                if(strcmp(thisLine,"") != 0)
                    print_uniq(prevLine,0);
            }else{
                print_uniq(prepre,0);
            }
        }
    }else{
        if(strcmp(thisLine,prevLine) != 0){ //thisline != prevLIne
            print_uniq(prevLine,0);
            count_dup = 1;
            if(strcmp(thisLine,"") != 0)
                print_uniq(thisLine,0);
        }else{ //thisLine == pervline
            if(strcmp(thisLine,prepre) != 0){
//                print_uniq(prepre,0);
                count_dup = 2;
                if(strcmp(thisLine,"") != 0)
                    print_uniq(prevLine,0);
            }else{
                print_uniq(prepre,0);
            }
        }
    }
}

int
main(int argc, char *argv[]) {
    int fd, i;
    char *name;
    int occur, dup, ig;
    occur=dup=ig=0;
    
    fd = 0;
    name = "";
    if (argc <= 1) {
        uniq(fd, name, occur,dup,ig);
        exit();
    }
    
    else {
        for (i = 0; i < argc; i++) {
            //            printf(1,"argv[i] %s\n",argv[i]);
            if(strcmp(argv[i],"-i") == 0){ // ignore case
                ig = 1;
                //                printf(1,"find i \n");
            }else if(strcmp(argv[i],"-c") == 0){ // count occurance
                occur = 1;
                //                    printf(1,"find c \n");
            }else if(strcmp(argv[i],"-d") == 0){ // count occurance
                dup = 1;
            }
            
            if (atoi(argv[i]) == 0 && *argv[i] != '0' && *argv[i] != '-') {
                if ((fd = open(argv[i], 0)) < 0) {
                    printf(1, "uniq: cannot open %s\n", argv[i]);
                    exit();
                }
            }
            else {
                argv[i]++;
            }
        }
        if(dup == 1 && occur == 1){
            printf(1, "uniq: -d -c cannot be execute in the same time %s\n", argv[i]);
            exit();
        }else{
            uniq(fd, name, occur,dup,ig);
            close(fd);
            exit();
        }
        
    }
}
