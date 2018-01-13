#include "cachelab.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define BIT 64

//global variable


typedef struct result
{
    int hit;
    int miss;
    int eviction;
} RESULT;

typedef struct param
{
    int verbose;
    int s;
    int E;
    int b;
    char* t;
} PARAM;
//structures
typedef struct line
{
    unsigned int time;
    int valid;
    long long tag;
} LINE;


int handle_arguments(int argc, char *argv[], PARAM *param_instance);
int printhelp();
int Load(char* address, int size, LINE**** workline);
int Save(char* address, int size, LINE**** workline);
int Modify(char* address, int size, LINE**** workline);
int Inst(char* address, int size);
int mem_parse(char* current_line, char** instruction, char** address, char** size);



int main(int argc, char *argv[])
{
    //initilization, sadly, want to avoid external global variables.
    RESULT *result_instance = malloc(sizeof(RESULT));
    result_instance -> hit = 0;
    result_instance -> miss = 0;
    result_instance -> eviction = 0;
    
    PARAM *param_instance = malloc(sizeof(PARAM));
    param_instance->verbose = 0;
    param_instance->s = 0;
    param_instance->E = 0;
    param_instance->b = 0;
    param_instance->t = "tracefile";
    //handle argument
    
    handle_arguments(argc, argv, param_instance);
    //printf("%d, %d, %d, %s\n", s, E, b, t);
    
    
    //initialize the model
    int S = 1 << (param_instance->s);
    //int B = 1 << b;
    
    LINE*** cache = malloc(S * sizeof(LINE**));
    if (cache == NULL) return -1;
    
    for (int i=0; i<S; i++)
    {
        cache[i] = malloc((param_instance->E) * sizeof(LINE*));
        if (cache[i] == NULL) return -1;
        for (int j=0; j<(param_instance->E);j++)
        {
            cache[i][j] = malloc(sizeof(LINE));
            if (cache[i][j] == NULL) return -1;
            cache[i][j]-> valid = 0;
        }
    }
    
    printf("LINE SIZE%lu\n", sizeof(LINE));
    
    //handle trace
    FILE *fptr = fopen(param_instance->t, "r");
    if (fptr == NULL)
    {
        printf("cannot open file\n");
        exit(0);
    }
    
    //parse setting
    char current_line[60];
    char* instruction = NULL;
    char* address = NULL; 
    char* size = NULL;
    
    while (fgets(current_line, 60, fptr) != NULL)
    {
        //parse a line
        mem_parse(current_line, &instruction, &address, &size);
        //memory model
        int size_N = atoi(size);
        //printf("%d", size_N);
        switch(instruction[0])
        {
            case 'I':
                Inst(address, size_N);
                break;
            case 'L':
                Load(address, size_N, &cache);
                break;
            case 'S':
                Save(address, size_N, &cache);
                break;
            case 'M':
                Modify(address, size_N, &cache);
                break;
            default:
                printf("ERROR\n");
        }
    }
    fclose(fptr);
    printf("%d, %s", param_instance->s, param_instance->t);
    printSummary(result_instance->hit, result_instance->miss, result_instance->eviction);
    return 0;
}

int mem_parse(char* current_line, char** instruction, char** address, char** size)
{ 
    *instruction = strtok(current_line, " ,");
    *address = strtok(NULL, " ,");
    *size = strtok(NULL, " ,");
    return 0;
}

int Load(char* address, int size, LINE**** workline)
{
    
    return 0;
}

int Save(char* address, int size, LINE**** workline)
{
    //printf("Save\n");
    return 0;
}

int Modify(char* address, int size, LINE**** workline)
{
    Load(address, size, workline);
    Save(address, size, workline);
    return 0;
}

int Inst(char* address, int size)
{
    printf("Instruction mem, do nothing\n");
    return 0;
}




int handle_arguments(int argc, char *argv[], PARAM *param_instance)
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
    
    while ((c = getopt(argc, argv, "hvs:E:b:t:")) !=-1)
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
            {
                s_flag = 1;
                s_tmp = optarg;
            }
                break;
            case 'E':
            {
                E_flag = 1;
                E_tmp = optarg;
            }
                break;
            case 'b':
            {
                b_flag = 1;
                b_tmp = optarg;
            }
                break;
            case 't':
            {
                t_flag = 1;
                t_tmp = optarg;
            }
                break;
            case '?':
                fprintf(stderr, "Unrecognized option: -%c\n", optopt);
                err_flag++;
                break;
            case ':':
                fprintf(stderr, "Option -%c requires an operand\n", optopt);
                err_flag++;
                break;
            default:
                fprintf(stderr, "Requires a useful operand\n");
                err_flag++;
                break;
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
            param_instance->verbose = 1;
        }
        if (s_flag*E_flag*b_flag*t_flag)
        {
            (param_instance)->s = atoi(s_tmp);
            (param_instance)->E = atoi(E_tmp);
            (param_instance)->b = atoi(b_tmp);
            (param_instance)->t = t_tmp;
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
