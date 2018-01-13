#include "cachelab.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define BIT 64
#define LAST(k,n) ((k) & ((1<<(n))-1))
#define MID(k,m,n) LAST((k)>>(m),((n)-(m)))
#define FRONT(k,n) ((k) >> (BIT-n))   //only for unsigned long long

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
    unsigned long long tag;
} LINE;


int handle_arguments(int argc, char *argv[], PARAM *param_instance);
int printhelp();
int Load(char* address, int size, LINE*** workline, PARAM *param_instance, RESULT *result_instance);
int Save(char* address, int size, LINE*** workline,PARAM *param_instance, RESULT *result_instance);
int Modify(char* address, int size, LINE*** workline, PARAM *param_instance, RESULT *result_instance);
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
                Load(address, size_N, cache, param_instance, result_instance);
                break;
            case 'S':
                Save(address, size_N, cache, param_instance, result_instance);
                break;
            case 'M':
                Modify(address, size_N, cache, param_instance, result_instance);
                break;
            default:
                printf("ERROR\n");
        }
    }
    fclose(fptr);
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

int Load(char* address, int size, LINE*** cache, PARAM *param_instance, RESULT *result_instance)
{
    unsigned long long address_num = (unsigned long long)strtol(address, NULL, 16);
    unsigned long long b_bit = LAST(address_num, param_instance->b);
    unsigned long long s_bit = MID(address_num, param_instance->b, param_instance->b + param_instance->s);
    unsigned long long tag_bit = FRONT(address_num, BIT-(param_instance->b + param_instance->s));
    int hit_flag = 0;
    int miss_flag = 0;
    int evict_flag = 0;
    int valid_flag = 0;
    
    LINE** current_set = cache[s_bit];
    //search hit
    for (int j=0; j<(param_instance->E);j++)
    {
        //check valid bit
        if (cache[s_bit][j]-> valid == 0) continue;
        //
        if (cache[s_bit][j]-> tag == tag_bit)
        {
            result_instance-> hit++;
            cache[s_bit][j]-> time = 0;
            hit_flag = 1;
        }
    }
    //search eviction
    if (hit_flag == 0)
    {
        result_instance-> miss++;
        miss_flag = 1;
        //search invalid
        for (int j=0; j<(param_instance->E);j++)
        {
            if (cache[s_bit][j]->valid == 0)
            {
                cache[s_bit][j]->valid = 1;
                cache[s_bit][j]->tag = tag_bit;
                cache[s_bit][j]->time = 0;
                valid_flag = 1;
                break;
            }
        }
        //search eviction
        if (valid_flag == 0)
        {
            result_instance->eviction++;
            evict_flag = 1;
            unsigned int longest_time = cache[s_bit][0]->time;
            unsigned int current_evict = 0;
            for (int j=0; j<(param_instance->E);j++)
            {
                if (cache[s_bit][j]->time > longest_time)
                {
                    longest_time = cache[s_bit][j]->time;
                    current_evict = j;
                }
            }
            //evict
            cache[s_bit][current_evict]->tag = tag_bit;
            cache[s_bit][current_evict]->time = 0;
        }
    }
    //update time
    int S = 1 << (param_instance->s);
    for (int i=0; i<S; i++)
    {
        for (int j=0; j<(param_instance->E);j++)
        {
            cache[i][j]->time++;
        }
    }
    //print single-operation summary
    printf("Load, %s, hit: %d, miss:%d, eviction:%d", param_instance->t ,hit_flag, miss_flag, evict_flag);
    return 0;
}

int Save(char* address, int size, LINE*** cache, PARAM *param_instance, RESULT *result_instance)
{
    //printf("Save\n");
    return 0;
}

int Modify(char* address, int size, LINE*** cache, PARAM *param_instance, RESULT *result_instance)
{
    Load(address, size, cache, param_instance, result_instance);
    Save(address, size, cache, param_instance, result_instance);
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
