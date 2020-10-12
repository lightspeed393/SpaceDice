#ifndef FUNCTIONS
#define FUNCTIONS
#include <stdint.h>

#define MAXBUF 2048
#define FILENAME "SPACE.conf"
#define DELIM "="

struct config
{
    char rpcuser[MAXBUF];
    char rpcpassword[MAXBUF];
    int rpcport;
};

struct t_getinfo {
    uint64_t blocks;
    uint64_t longestchain;
    double balance;
};

struct t_diceinfo {
   char fundingtxid[65]; // "5be49570c56d036abb08b6d084da93a8a86f58fc48db4a1086be95540d752d6f",
   char name[65]; // "KMDICE",
   uint64_t sbits; // 76155294338379,
   double minbet; // "0.00100000",
   double maxbet; // "1000.00000000",
   uint64_t maxodds; // 1500,
   uint64_t timeoutblocks; // 600,
   double funding; // "10271387.84870000"
};

t_getinfo getInfo();
t_diceinfo getDiceInfo();
char* dicebet(char *rawtx, int len, char *name, char* fundingtxid, double amount, uint64_t odds);
char* sendrawtx(char *txid, char *rawtx, int rawtx_len);
int dicestatus(char *txid, double *won, char *name, char* fundingtxid);

char *trimwhitespace(char *str);
struct config get_config(char *filename);
void init_config();

#endif // FUNCTIONS
