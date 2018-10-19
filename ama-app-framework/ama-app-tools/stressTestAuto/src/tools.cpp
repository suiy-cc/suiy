/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include "tools.h"

#define PROC_DIRECTORY "/proc/"
#define CASE_SENSITIVE    1
#define CASE_INSENSITIVE  0
#define EXACT_MATCH       1
#define INEXACT_MATCH     0

#define DELAY10MS (10*1000)
#define DELAY5MS (5*1000)

struct timeval sTimeVal;

void delayAfterCallAPI(void){
    usleep(DELAY10MS);
}

//是不是数字
int IsNumeric(const char* ccharptr_CharacterList)
{
    for ( ; *ccharptr_CharacterList; ccharptr_CharacterList++)
        if (*ccharptr_CharacterList < '0' || *ccharptr_CharacterList > '9')
            return 0; // false
    return 1; // true
}

//intCaseSensitive=0大小写不敏感
int strcmp_Wrapper(const char *s1, const char *s2, int intCaseSensitive)
{
    if (intCaseSensitive)
        return !strcmp(s1, s2);
    else
        return !strcasecmp(s1, s2);
}

//intCaseSensitive=0大小写不敏感
int strstr_Wrapper(const char* haystack, const char* needle, int intCaseSensitive)
{
    // if (intCaseSensitive)
    //     return (int) strstr(haystack, needle);
    // else
        // return (int) strcasestr(haystack, needle);
        return 1;
}


#ifdef __cplusplus
pid_t GetPIDbyName(const char* cchrptr_ProcessName, int intCaseSensitiveness, int intExactMatch)
#else
pid_t GetPIDbyName_implements(const char* cchrptr_ProcessName, int intCaseSensitiveness, int intExactMatch)
#endif
{
    char chrarry_CommandLinePath[100]  ;
    char chrarry_NameOfProcess[300]  ;
    char* chrptr_StringToCompare = NULL ;
    pid_t pid_ProcessIdentifier = (pid_t) -1 ;
    struct dirent* de_DirEntity = NULL ;
    DIR* dir_proc = NULL ;

    int (*CompareFunction) (const char*, const char*, int) ;

    if (intExactMatch)
        CompareFunction = &strcmp_Wrapper;
    else
        CompareFunction = &strstr_Wrapper;


    dir_proc = opendir(PROC_DIRECTORY) ;
    if (dir_proc == NULL)
    {
        perror("Couldn't open the " PROC_DIRECTORY " directory") ;
        return (pid_t) -2 ;
    }

    // Loop while not NULL
    while ( (de_DirEntity = readdir(dir_proc)) )
    {
        if (de_DirEntity->d_type == DT_DIR)
        {
            if (IsNumeric(de_DirEntity->d_name))
            {
                strcpy(chrarry_CommandLinePath, PROC_DIRECTORY) ;
                strcat(chrarry_CommandLinePath, de_DirEntity->d_name) ;
                strcat(chrarry_CommandLinePath, "/cmdline") ;
                FILE* fd_CmdLineFile = fopen (chrarry_CommandLinePath, "rt") ;  //open the file for reading text
                if (fd_CmdLineFile)
                {
                    int count = fscanf(fd_CmdLineFile, "%s", chrarry_NameOfProcess) ; //read from /proc/<NR>/cmdline
                    fclose(fd_CmdLineFile);  //close the file prior to exiting the routine

                    if (strrchr(chrarry_NameOfProcess, '/'))
                        chrptr_StringToCompare = strrchr(chrarry_NameOfProcess, '/') +1 ;
                    else
                        chrptr_StringToCompare = chrarry_NameOfProcess ;

                    //printf("Process name: %s\n", chrarry_NameOfProcess);
                    //这个是全路径，比如/bin/ls
                    //printf("Pure Process name: %s\n", chrptr_StringToCompare );
                    //这个是纯进程名，比如ls

                    //这里可以比较全路径名，设置为chrarry_NameOfProcess即可
                    // printf("chrptr_StringToCompare: %s\n", chrptr_StringToCompare);
                    // printf("cchrptr_ProcessName: %s\n", cchrptr_ProcessName);
                    if ( CompareFunction(chrptr_StringToCompare, cchrptr_ProcessName, intCaseSensitiveness) )
                    {
                        pid_ProcessIdentifier = (pid_t) atol(de_DirEntity->d_name) ;
                        closedir(dir_proc) ;
                        return pid_ProcessIdentifier ;
                    }
                }
            }
        }
    }
    closedir(dir_proc) ;
    return pid_ProcessIdentifier ;
}

#ifdef __cplusplus
    pid_t GetPIDbyName(const char* cchrptr_ProcessName)
    {
        return GetPIDbyName(cchrptr_ProcessName, CASE_INSENSITIVE, EXACT_MATCH) ;
    }
#else
    // C cannot overload functions - fixed
    /*这里是原文的实现，但貌似需要某些牛X的设置，因不需要，删之
    pid_t GetPIDbyName_Wrapper(const char* cchrptr_ProcessName, ... )
    {
        int intTempArgument ;
        int intInputArguments[2] ;
        // intInputArguments[0] = 0 ;
        // intInputArguments[1] = 0 ;
        memset(intInputArguments, 0, sizeof(intInputArguments) ) ;
        int intInputIndex ;
        va_list argptr;

        va_start( argptr, cchrptr_ProcessName );
            for (intInputIndex = 0;  (intTempArgument = va_arg( argptr, int )) != 15; ++intInputIndex)
            {
                intInputArguments[intInputIndex] = intTempArgument ;
            }
        va_end( argptr );
        return GetPIDbyName_implements(cchrptr_ProcessName, intInputArguments[0], intInputArguments[1]);
    }

    #define GetPIDbyName(ProcessName,...) GetPIDbyName_Wrapper(ProcessName, ##__VA_ARGS__, (int) 15)
    */
    //简单实现
    pid_t GetPIDbyName_Wrapper(const char* cchrptr_ProcessName)
    {
            return GetPIDbyName_implements(cchrptr_ProcessName, 0,0);//大小写不敏感
    }
#endif

pid_t getPidOfProc(const char* procName){
    pid_t pid = GetPIDbyName(procName) ; // If -1 = not found, if -2 = proc fs access error
    printf("PID %d\n", pid);
    return pid;
}

bool isProcOfPidExist(pid_t pid) {
    // char proc_pid_path[300];
    // sprintf(proc_pid_path, "/proc/%d/status", pid);
    // FILE* fp = fopen(proc_pid_path, "r");
    // if(NULL != fp){
    //     return 1;
    // }else{
    //     return 0;
    // }
    int ret = kill(pid,0);
    if(0==ret){
        return true;
    }else{
        return false;
    }
}

int getRand(int min,int max){
    gettimeofday(&sTimeVal,NULL);
    // srand((unsigned)time(NULL));
    srand((unsigned)sTimeVal.tv_usec);
	int ret = (rand() % (max-min+1))+ min;
	return ret;
}
