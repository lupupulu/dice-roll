#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#if defined(_WIN64) || defined(_WIN32)
#include <windows.h>
#include <ntsecapi.h>
#else
#include <sys/random.h>
#endif
#include <time.h>
#include <unistd.h>

#define ABS(x) ((x)>0?(x):(-x))

void print_help(void);
unsigned int to_op(const char *str);
int64_t operator(unsigned int op,int64_t origin,int64_t arg);
unsigned int get_true_random(void);
int64_t dice_round(int64_t v);

int show_detail=0;
int show_bool=0;
int cmp_higher=0;
int cmp_great_success=0;
int cmp_major_failure=0;
int round_up=0;

int64_t last_dice=0,last_dice_n=0,last_dice_v=0;

int main(int argc,const char **argv){
    int64_t res=0,num=0;
    unsigned int op=0,op_l=0;

    if(argc<=1){
        print_help();
        return 1;
    }

    int i=2;
    op=to_op(argv[1]);
    switch(op){
    case -1:
        printf(
            "\033[31;1mYou can not use %s as the first argument!\033[0m\n",
            argv[1]
        );
        return 1;
        break;
    case 'h':
        print_help();
        return 0;
        break;
    case 'l':
        op_l=1;
        if(argc>2){
            op=to_op(argv[2]);
            sscanf(argv[2],"%ld",&res);
            res*=100;
            i=3;
        }
        break;
    case 'c':
        show_bool=1;
        break;
    }
    if(
        op=='*'||
        op=='d'||
        op=='a'||
        op=='i'
    ){
        res=100;
    }
    if(!op&&!op_l){
        if(!sscanf(argv[1],"%ld",&res)){
            printf(
                "\033[31;1mYou can not use %s as the first argument!\033[0m\n",
                argv[1]
            );
            return 1;
        }
        res*=100;
    }

    for(;i<argc;i++){
        if(argv[i][0]>='0'&&argv[i][0]<='9'){
            if(!sscanf(argv[i],"%ld",&num)){
                printf(
                    "\033[31;1m%s is not a supported number.\033[0m\n",
                    argv[i]
                );
                return 1;
            }
            res=operator(op,res,num);
            op=0;
        }else if(!op){
            op=to_op(argv[i]);
            if(!op||op==-1){
                printf(
                    "\033[31;1m%s is not a supported operator.\033[31;0m\n",
                    argv[i]
                );
                return 1;
            }
        }else{
            return 1;
        }
    }

    if(show_detail){
        printf("\033[1mresult = ");
    }

    if(show_bool){
        if(res){
            if(show_detail){
                printf("\033[32m");
            }
            printf("success\n");
        }else{
            if(show_detail){
                printf("\033[31m");
            }
            printf("failure\n");
        }
        if(show_detail){
            printf("\033[0m");
        }
        return 0;
    }
    if(res%100){
        printf("%ld.%ld\n",res/100,ABS(res%100));
        if(show_detail){
            if(round_up){
                printf("\033[4mround up:   %ld\033[0m\n",res/100+1);
                printf("\033[1mround down: %ld\n",res/100);
            }else{
                printf("\033[4mround down: %ld\033[0m\n",res/100);
                printf("\033[1mround up:   %ld\n",res/100+1);
            }
        }
    }else{
        printf("%ld\n",dice_round(res));
    }

    if(show_detail){
        printf("\033[0m");
    }
    

    return 0;
}

int64_t operator(unsigned int op,int64_t origin,int64_t arg){
    int64_t ret=0;

    int64_t ext=0,extn=0;

    show_bool=0;

    int64_t great_succes=0,major_failure=0;

    switch(op){
    case 'd':
        if(show_detail){
            printf(
                "\033[1mROLL DICES %ldd%ld:\033[0m\n",
                dice_round(origin),
                arg
            );
        }
        for(register int64_t i=0;i<dice_round(origin);i++){
            unsigned int randn=get_true_random()%arg+1;

            if(show_detail){
                printf("\t[ %2ld ]: %d\n",i+1,randn);
            }
            ret+=randn;
        }
        last_dice=arg;
        last_dice_n=dice_round(origin);
        last_dice_v=ret;
        return ret*100;
        break;

    case 'a':
        if(show_detail){
            printf(
                "\033[1mROLL DICES %ldd%ld with advantage:\033[0m\n",
                dice_round(origin),
                arg
            );
        }

        ext=arg+1;
        for(register int64_t i=0;i<dice_round(origin)+1;i++){
            unsigned int randn=get_true_random()%arg+1;
            if(show_detail){
                printf("\t[ %2ld ]: %d\n",i+1,randn);
            }
            if(ext>randn){
                extn=i+1;
                ext=randn;
            }

            ret+=randn;
        }

        if(show_detail){
            printf(
                "\033[1m"
                "\tabandon \'DICE[%2ld]: %ld\', final result is %ld\n"
                "\033[0m",
                extn,
                ext,
                ret-ext
            );
        }

        last_dice=arg;
        last_dice_n=dice_round(origin);
        last_dice_v=ret-ext;
        return (ret-ext)*100;
        break;

    case 'i':
        if(show_detail){
            printf(
                "\033[1mROLL DICES %ldd%ld with disadvantage:\033[0m\n",
                dice_round(origin),
                arg
            );
        }

        ext=0;
        for(register int64_t i=0;i<dice_round(origin)+1;i++){
            unsigned int randn=get_true_random()%arg+1;
            if(show_detail){
                printf("\t[ %2ld ]: %u\n",i+1,randn);
            }
            if(ext<randn){
                extn=i+1;
                ext=randn;
            }

            ret+=randn;
        }

        if(show_detail){
            printf(
                "\033[1m"
                "\tabandon \'DICE[%2ld]: %ld\', final result is %ld\n"
                "\033[0m",
                extn,
                ext,
                ret-ext
            );
        }

        last_dice=arg;
        last_dice_n=dice_round(origin);
        last_dice_v=ret-ext;
        return (ret-ext)*100;
        break;

    case 'c':
        show_bool=1;
        if(!last_dice){
            goto L;
        }
        if(cmp_higher){
            great_succes=last_dice_n;
            major_failure=last_dice_n*last_dice;
        }else{
            great_succes=last_dice_n*last_dice;
            major_failure=last_dice_n;
        }
        if(cmp_great_success&&last_dice_v==great_succes){
            if(show_detail){
                printf("\033[1;32mBIG SUCCESS!\033[0m\n");
            }
            return 100;
        }else if(cmp_major_failure&&last_dice_v==major_failure){
            if(show_detail){
                printf("\033[1;31mMAJOR FAILURE!\033[0m\n");
            }
            return 0;
        }
        L:
        if(cmp_higher){
            ret=(dice_round(origin)<=arg)*100;
            if(show_detail){
                if(ret){
                    printf(
                        "\033[1;32msuccess:\033[0m %ld <= %ld\n",
                        dice_round(origin),
                        arg
                    );
                }else{
                    printf(
                        "\033[1;31mfailure:\033[0m %ld > %ld\n",
                        dice_round(origin),
                        arg
                    );
                }
            }
        }else{
            ret=(dice_round(origin)>=arg)*100;
            if(show_detail){
                if(ret){
                    printf(
                        "\033[1;32msuccess:\033[0m %ld >= %ld\n",
                        dice_round(origin),
                        arg
                    );
                }else{
                    printf(
                        "\033[1;31mfailure:\033[0m %ld < %ld\n",
                        dice_round(origin),
                        arg
                    );
                }
            }
        }
        return ret;
        break;

    case '+':
        return origin+arg*100;
        break;
    case '-':
        return origin-arg*100;
        break;
    case '*':
        return origin*arg;
    case '/':
        return origin/arg;
        break;
    }

    return 0;
}

#if defined(_WIN64) || defined(_WIN32)
unsigned int get_true_random(void){
    unsigned int ret;
    if(!RtlGenRandom(&ret,4)){
        ret=rand();
        printf("\033[1;31mfailed to use RtlGenRandom.\033[0m\n");
    }
    return ret;
}
#else
unsigned int get_true_random(void){
    unsigned int seed;
    if(getrandom(&seed,sizeof(seed),GRND_NONBLOCK)!=sizeof(seed)){
        printf("\033[1;31mfailed to use getrandom.\033[0md\n");
        return (unsigned int)time(NULL)+getpid();
    }
    return seed;
}
#endif

unsigned int to_op(const char *str){
    unsigned int op=0;
    switch(str[0]){
    case '-':
        switch(str[1]){
        case '\0':
            op='-';
            break;
        case 'h':
            switch(str[2]){
            case '\0':
                op='h';
                break;
            default:
                return -1;
                break;
            }
            break;
        default:
            op=-1;
            for(int i=1;str[i]!='\0';i++){
                switch(str[i]){
                case 'l':
                    show_detail=1;
                    op='l';
                    break;
                case 'g':
                    cmp_higher=1;
                    op='l';
                    break;
                case 's':
                    cmp_great_success=1;
                    op='l';
                    break;
                case 'f':
                    cmp_major_failure=1;
                    op='l';
                    break;
                case 'u':
                    round_up=1;
                    op='l';
                    break;
                }
            }
            break;
        }
        break;
    case '+':
        op='+';
        if(str[1]!='\0'){
            return 0;
        }
        break;
    case '*':
        op='*';
        if(str[1]!='\0'){
            return 0;
        }
        break;
    case '/':
        op='/';
        if(str[1]!='\0'){
            return 0;
        }
        break;
    case 'd':
        switch(str[1]){
        case '\0':
            op='d';
            break;
        case 'c':
            switch(str[2]){
            case '\0':
                op='c';
                break;
            }
            break;
        case 'i':
            if(!strcmp(str,"dis")){
                op='i';
            }
            break;
        }
        break;
    case 'a':
        if(!strcmp(str,"adv")){
            op='a';
        }else{
            return 0;
        }
        break;
    }
    return op;
}

int64_t dice_round(int64_t v){
    if(round_up&&v%100){
        return v/100+1;
    }
    return v/100;
}

void print_help(void){
    printf("Usage:");
    printf(" roll [options] [initial_value] [operations...]\n\n");
    printf("Options:\n");
    printf("  -l          Show detailed operation process\n");
    printf("  -c          Show boolean result (success/failure)\n");
    printf("  -g          Higher comparison (for dc)\n");
    printf("  -s          Enable great success detection\n");
    printf("  -f          Enable major failure detection\n");
    printf("  -u          Round up results\n");
    printf("  -h          Show this help message\n\n");
    printf("Operations:\n");
    printf("  + [num]     Add number\n");
    printf("  - [num]     Subtract number\n");
    printf("  * [num]     Multiply by number\n");
    printf("  / [num]     Divide by number\n");
    printf("  d [num]     Roll dice: XdY\n");
    printf("  dc [num]    Dice check against target number\n");
    printf("  adv [num]   Advantage: roll X+1, take highest X\n");
    printf("  dis [num]   Disadvantage: roll X+1, take lowest X\n\n");
    printf("Examples:\n");
    printf("  roll 5 d 6          # Roll 5d6\n");
    printf("  roll -l 2 adv 20    # Advantage roll with details\n");
    printf("  roll 15 dc 10       # Check if result beats target 10\n");
    printf("  roll -h -s -f 5 d 20 dc 15  # Complex check with options\n");
}