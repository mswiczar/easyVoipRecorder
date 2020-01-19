#include "CCmdLineArgs.h"
#include <string.h>

/* -- Shared Global -- */
int CCmdLineArgs::optind = 1;
char *CCmdLineArgs::optarg = NULLP;
char *CCmdLineArgs::optopt = NULLP;

/* -- Private -- */
char CCmdLineArgs::GetLongOption(const lopt_entry *lopts)
{
    /* -- If no long option table is provided. -- */
    if (lopts == NULLP) {
        return CLA_INVALID;
    }
    /* -- Blank long option name. -- */
    if (optopt[0] == NULLP) {
        return CLA_INVALID;
    }  
    int i = 0;
    /* -- Cycle through each entry in the long option table. -- */    
    while (lopts->name != NULLP) {
        if (strstr(optopt, lopts->name) == optopt) {        
        /* -- Long option name valid. -- */
            /* -- Argument to option should be the next character on. -- */        
            optarg = optopt+strlen(lopts->name);
            /* -- Check has_arg. -- */
            switch (lopts->has_arg) {
                case CLA_ARG:
                    /* -- Must have an argument, but argument is blank. -- */
                    if (optarg[0] == NULLP) {
                        return CLA_MISSING;
                    }
                    break;
                case CLA_NOARG:
                    /* -- Must not have an argument, but argument is not blank. -- */
                    if (optarg[0] != NULLP) {
                        return CLA_INVALID;
                    }
                    break;                    
                case CLA_OPTARG:
                    /* -- Whether argument is blank, both cases are valid. -- */
                    break;
            }
            /* -- If flag is not NULL, set *flag to val. -- */
            if (lopts->flag) {
                *lopts->flag = lopts->val;
                return 0;
            }
            /* -- Flag is NULL, return val as a character. -- */
            return lopts->val;
        }      
        i++;
        lopts++;
    }
    /* -- Not a valid long option name. -- */
    return CLA_INVALID;
}

char CCmdLineArgs::GetShortOption(const char *optstr)
{
    /* -- No short option string is given. -- */
    if (optstr == NULLP) {
        /* -- All short option is invalid. -- */
        return CLA_INVALID;
    }
    /* -- Short option is blank. -- */
    if (optopt[0] == NULLP) {
        return CLA_INVALID;
    }
    char *chr;
    int has_arg;
    if ((chr = strchr(optstr, optopt[0]))) {
    /* -- Short option name valid. -- */
        /* -- Get has_arg. -- */
        has_arg = CLA_NOARG;
        if (chr[1] == ':') {
            has_arg = CLA_ARG;
            if (chr[2] == ':') {
                has_arg = CLA_OPTARG;
            }
        }
        /* -- Argument to option should be from the next character on. -- */
        optarg = optopt+1;
        switch (has_arg) {
            case CLA_ARG:
                if (optarg[0] == NULLP) {
                    return CLA_MISSING;
                }
                break;
            case CLA_NOARG:
                if (optarg[0] != NULLP) {
                    return CLA_INVALID;
                }
                break;
            case CLA_OPTARG:
                break;
        }
        /* -- Return short option character. -- */
        return optopt[0];
    }
    /* -- Invalid short option. -- */
    return CLA_INVALID;
}

/* -- Public -- */
char CCmdLineArgs::Parse(int argc, char *const argv[], const char *optstr, const lopt_entry *lopts)
{
    if (optind < argc) {
        char ret;
        optopt = argv[optind];       
        if (optopt[0] == '-') {
            optopt++;
            if (optopt[0] == '-') {
                optopt++;
                /* -- Long option. -- */
                ret = GetLongOption(lopts);
            }
            else {
                /* -- Short option. -- */
                ret = GetShortOption(optstr);
            }
        }
        else {
            /* -- Encounter the first non-option argument. -- */
            return CLA_DONE;
        }
        optind++;
        return ret;
    }
    else {
        /* -- No more arguments left. -- */
        return CLA_DONE;
    }
}