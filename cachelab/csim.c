#include "cachelab.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>


int handle_arguments(char *argv[], int *s, int *E, int *b, char** t, int argc, int *verbose);
int printhelp();

int main(int argc, char *argv[])
{
    int verbose = 0;
    int s = 0;
    int E = 0;
    int b = 0;
    char* t = "helloa";
    
    
    handle_arguments(argv, &s, &E, &b, &t, argc, &verbose);
    printf("%d, %d, %d, %s\n", s, E, b, t);
    printf("%d", verbose);
    printSummary(0, 0, 0);
    return 0;
}

int handle_arguments(char *argv[], int *s, int *E, int *b, char** t, int argc, int *verbose)
{
    int h_flag = 0;
    int v_flag = 0;
    int s_flag = 0;
    int E_flag = 0;
    int b_flag = 0;
    int t_flag = 0;
    
    char* s_tmp = "no";
    char* E_tmp = "no";
    char* b_tmp = "no";
    char* t_tmp = "notrace";
    
    int c = 0;
    int err_flag = 0;
    
    while ((c = getopt(argc, argv, "hvsEbt")) !=-1)
    {
        
        switch(c)
        {
            case 'h':
                h_flag = 1;
                break;
            case 'v':
                v_flag = 1;
                break;
            case 's':
                s_flag = 1;
                s_tmp = optarg;
                break;
            case 'E':
                E_flag = 1;
                E_tmp = optarg;
                break;
            case 'b':
                b_flag = 1;
                b_tmp = optarg;
                break;
            case 't':
                t_flag = 1;
                t_tmp = optarg;
                break;
            case '?':
                fprintf(stderr, "Unrecognized option: -%c\n", optopt);
                err_flag++;
            case ':':
                fprintf(stderr, "Option -%c requires an operand\n", optopt);
                err_flag++;
            default:
                fprintf(stderr, "Requires a useful operand\n");
                err_flag++;
        }
    }
    //handle flags
    if (err_flag)
        {
            printhelp();
            exit(2);
        }
        if (h_flag)
        {
            printhelp();
            return 1;
        }
        if (v_flag)
        {
            *verbose = 1;
        }
        if (s_flag*E_flag*b_flag*t_flag)
        {
            *s = atoi(s_tmp);
            *E = atoi(E_tmp);
            *b = atoi(b_tmp);
            *t = t_tmp;
        }
        else
        {
            printhelp();
            return 1;
        }
    
    return 0;
}


int printhelp()
{
    printf("\n"
    "Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n"
    "Options:\n"
    "-h         Print this help message.\n"
    "-v         Optional verbose flag.\n"
    "-s <num>   Number of set index bits.\n"
    "-E <num>   Number of lines per set.\n"
    "-b <num>   Number of block offset bits.\n"
    "-t <file>  Trace file.\n"
    "Examples:\n"
    "linux>  ./csim -s 4 -E 1 -b 4 -t traces/yi.trace\n"
    "linux>  ./csim -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
    return 0;
}




//printf("%s, %s, %s, %s\n", argv[1], argv[2], argv[3], argv[4]);
