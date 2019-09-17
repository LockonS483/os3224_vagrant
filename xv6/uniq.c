#include "types.h"
#include "stat.h"
#include "user.h"

char buf[512];


char*
tolower(char *ss)
{
  int i;
  int n = strlen(ss);
  char *s = malloc(n);
  memmove(s, ss, n);
  char *l = malloc(n);
  for(i=0; i<n; i++){
    char c = s[i];
    if(c > 'A' && c < 'Z'){
      int diff = c - 'A';
      c = 'a' + diff;
    }
    l[i] = c;
  }

  return l;
}

void
uniqq(int fd, char *name, char o){
    int i, j, n;
    int match = 1;
    int count, outind = 0;
    char *out = malloc(4096);
    char *lastLine;
    char *thisLine = malloc(1024);

    int lastGroupInd = 0;

    int cCount = 1;

    while((n = read(fd, buf, sizeof(buf))) > 0){
        for(i=0; i<n; i++){
            thisLine[count] = buf[i];
            if(thisLine[count] == '\n' || i==(n-1)){
                //printf(1, "comparing: \n");
                //printf(1, lastLine);
                //printf(1, thisLine);
                if(thisLine[count] != '\n'){
                    thisLine[count+1] = '\n';
                }
                if(strlen(lastLine) == strlen(thisLine)){
                    match = 1;
                    if(o != 'i'){
                        if(strcmp(lastLine, thisLine) != 0){
                            match = 0;
                        }else{
                            cCount++;
                        }
                    }else{
                        //printf(1, "comparing with -i\n");
                        char *one = tolower(lastLine);
                        char *two = tolower(thisLine);
                        if(strcmp(one, two) != 0){
                            match = 0;
                        }else{
                            cCount++;
                        }
                    }
                    /*for(j=0; j<strlen(thisLine); j++){
                        if(lastLine[j] != thisLine[j]){
                            match = 0;
                            break;
                        }
                    }*/
                }else{
                    match = -1;
                }
                //printf(1, "Match: %d\n", match);

                if(match < 1){
                    if(o == 'c'){
                        out[lastGroupInd] = (cCount + '0');
                        lastGroupInd = outind;
                        out[outind++] = (cCount + '0');
                        out[outind++] = ' ';
                        cCount = 1;
                    }
                    if(o != 'd'){
                        for(j=0; j<strlen(thisLine); j++){
                            out[outind] = thisLine[j];
                            outind++;
                        }
                    }else{
                        if(cCount > 1){
                            for(j=0; j<strlen(lastLine); j++){
                                out[outind] = lastLine[j];
                                outind++;
                            }
                            cCount = 1;
                        }
                    }
                }
                lastLine = thisLine;
                thisLine = malloc(1024);
                count = 0;
            }else{
                count++;
            }
        }
    }

    if(n<0){
        printf(1, "uniq: read error\n");
        exit();
    }
    //printf(1, "\nOutput:\n");
    printf(1, out);
    printf(1, "\n");
}

int
main(int argc, char *argv[]){
    int fd, i;
    int st = 1;
    char option = 'f'; //f = default
    if(argc <= 1){
        uniqq(0, "", option);
        exit();
    }

    if(argc > 2){
        if(argv[1][0] == '-'){
            option = argv[1][1];
            st = 2;
        }
    }

    for(i=st; i<argc; i++){
        if((fd = open(argv[i], 0)) < 0){
            printf(1, "uniq: cannot open %s\n", argv[i]);
            exit();
        }
        uniqq(fd, argv[i], option);
        close(fd);
    }

    exit();
}