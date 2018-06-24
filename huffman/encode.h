#include "common.h" 

#define MAX_SYMBOLS 256
#define MAX_LEN     7

struct tnode
{
    struct  tnode* left; /*used when in tree*/
    struct  tnode*right; /*used when in tree*/  
    struct  tnode*parent;/*used when in tree*/
    struct  tnode* next; /*used when in list*/ 
    float   freq;
    int     isleaf;
    unsigned char     symbol;
};
struct tnode* talloc(int symbol,float freq);
void calc_freq(FILE* input);
void pq_insert(struct tnode* p);
struct tnode* pq_pop();
void generate_code(struct tnode* root,int depth);
void dump_code(FILE* fp);
void encode(unsigned char* str,FILE* fout);
void freetree(struct tnode* root);
void freelist(struct tnode* root);
