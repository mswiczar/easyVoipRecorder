#ifndef CCmdLineArgs_h_defined
#define CCmdLineArgs_h_defined

#define NULLP   0

/* --  Entry structure for long option. -- */
typedef struct lopt_entry_
{
    const char *name;
    int has_arg;
    int *flag;
    int val;
} lopt_entry;

/* -- Static class -- */
class CCmdLineArgs
{
    private:
        CCmdLineArgs(){}
        static char GetLongOption(const lopt_entry *lopts);
        static char GetShortOption(const char *optstr);
       
    public:
        static int optind;
        static char *optarg, *optopt;        
        static const char CLA_MISSING = ':';
        static const char CLA_INVALID = '?';
        static const char CLA_DONE = -1;
        enum {
            CLA_ARG, CLA_NOARG, CLA_OPTARG
        };        
        static char Parse(int argc, char *const argv[], const char *optstr, const lopt_entry *lopts);
};

#endif