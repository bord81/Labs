#include "encode.h"

/*global variables*/
static uint32_t code[MAX_SYMBOLS];
static unsigned char code_length[MAX_SYMBOLS];
static struct tnode* root=NULL; /*tree of symbols*/
static struct tnode* qhead=NULL; /*list of current symbols*/
static float freq[256];
static unsigned char   NCHAR;
static int symbols = 0;
static long curr_inp_s = 1024L;
static unsigned char* input_data=NULL;
static o_fsize o_file = EIGHT;
static jmp_buf err;
/*
    @function   talloc
    @desc       allocates new node 
*/
struct tnode* talloc(int symbol,float freq)

{
    struct tnode* p=(struct tnode*)malloc(sizeof(struct tnode));
    if(p!=NULL)
    {
        p->left=p->right=p->parent=p->next=NULL;
        p->symbol=symbol;
        p->freq=freq;
		p->isleaf=1;
    } else {
		longjmp(err, 1);
	}
    return p;
}

void calc_freq(FILE* input) 
{
	unsigned char c;
	int i;
	long inp_off = 0L;
	unsigned char* temp_p=NULL;
	long c_inp_s = curr_inp_s * 3 / 4;
	NCHAR = 0;
	input_data=malloc(curr_inp_s);
	if(input_data==NULL) {
		longjmp(err, 1);
	}
	for (i = 0; i < 256; i++){
		*(freq + i) = 0;
	}
	while((c = fgetc(input))!=0xFF)
	{
        if(*(freq + c) == 0) {
    		NCHAR++;
        }
		if((++symbols)>c_inp_s) {
			curr_inp_s *= 2;
			c_inp_s = curr_inp_s * 3 / 4;
			temp_p = realloc(input_data, curr_inp_s);
			if (temp_p == NULL) {
				longjmp(err, 1);
			}
		input_data = temp_p;
		} 
		*(freq + c) += 1;
		*(input_data + inp_off) = c;
		++inp_off;
	}
	for (i = 0; i < 256; i++){
		*(freq + i) = *(freq + i) / symbols;
	}			
}

/*
    @function pq_insert
    @desc     inserts an element into the priority queue
    NOTE:     makes use of global variable qhead
*/
void pq_insert(struct tnode* p)
{
    struct tnode* curr=NULL;
    struct tnode* prev=NULL;
   if(qhead==NULL) /*qhead is null*/
   {
   		qhead=p;
   		return;
   }
   curr = qhead;
   while(curr!=NULL){
	   if(qhead->freq > p->freq){
		   curr=qhead;
		   break;
	   } else if(curr->freq > p->freq){
		   break;
	   } 
	   if(curr->next!=NULL){
	   prev=curr;
	   curr=curr->next;
	   } else {
		   curr->next=p;
		   return;
	   }
   }
   if(curr==qhead)
   {
   	 	p->next=qhead;
   	 	qhead=p;
   }
   else /*insert between prev and next*/
   {
	 	p->next=curr;
	 	prev->next=p;
   }
}

/*
    @function pq_pop
    @desc     removes the first element
    NOTE:     makes use of global variable qhead
*/

struct tnode* pq_pop()
{
    struct tnode* p=NULL;
    p = qhead;
    if(p==NULL){
		return NULL;
	}
    qhead = qhead->next;
    return p;
}

/*
	@function build_code
	@desc     generates the string codes given the tree
	NOTE: makes use of the global variable root
*/
void generate_code(struct tnode* root,int depth)
{
	unsigned int symbol;
	unsigned int code_len;
	int len; /*length of code*/
    int i;
    unsigned int rev_bits;
	if(root->isleaf)
	{
		symbol=root->symbol;
		len   =depth;
		/*start backwards*/
		code[symbol]=0;
		struct tnode* current=root;
		struct tnode* up=root;
		code_len=0;
		while(len>-1) {
		up = up->parent;
		if(up==NULL){
			break;
		}
		if(up->left!=NULL && up->left==current){
			code[symbol] &= ~(1 << (len - 1));
			code_len++;
		}
		if(up->right!=NULL && up->right==current){
			code[symbol] |= 1 << (len - 1);
			code_len++;
		}
			current=up;
			len--;
		}
		rev_bits = 0;
        for (i = 0; i < code_len; i++){
        if((code[symbol] & (1 << i)))
           rev_bits |= 1 << ((code_len - 1) - i);  
        }
        code[symbol] = rev_bits;
		code_length[symbol]=code_len;
	}
	else
	{
		generate_code(root->left,depth+1);
		generate_code(root->right,depth+1);
	}
	
}

/*
	@func	dump_code
	@desc	output code file
*/
void dump_code(FILE* fp)
{
	int i=0;
	int j=0;
	int x=0; 
    fputc(NCHAR, fp);
	for(i=0;i<MAX_SYMBOLS;i++)
	{
		if(code_length[i]) /*non empty*/{			
			fputc(i, fp);
			for(j=8; j <= 32 ;j+=8){
			        x = code[i] >> (32 - j);
			        fputc(x, fp);
		        }

			fputc(code_length[i], fp);
		} 
	}
}

/*
	@func	encode
	@desc	outputs compressed stream
*/
void encode(unsigned char* str,FILE* fout)
{
	unsigned char* temp = str;
	unsigned char next_enc_char = 0;
	unsigned char fs = 0;
	unsigned char bit_index = 0;
	int i = 0, j = 0;
	int64_t file_size = 0L;
	int64_t real_size = 0L;

    while(*str) {
		file_size++;
		str++;
	}
	unsigned char* enc_out = malloc(sizeof(int)*file_size);
	unsigned char* enc_out_begin =enc_out;
	while(*temp) {
		for(i=0; i<code_length[*temp]; i++) {
			if(code[*temp] & (1<< (code_length[*temp] - i))) {
				next_enc_char |= 1 << (8 - bit_index);
			}
			if(bit_index < 8){
				bit_index++;
			} else {
				*enc_out = next_enc_char;
				enc_out++;
				real_size++;
				next_enc_char = 0;
				bit_index = 0;
				i--;
			}
		}
		if(code[*temp] & 1) {	
				next_enc_char |= 1 << (8 - bit_index);	
			}
		temp++;
		
	}
	if(next_enc_char) {
		*enc_out = next_enc_char;	
		enc_out++;
		real_size++;
		}
	*enc_out = '\0';
	dump_code(fout);
	if(file_size<UINT8_MAX) {
		o_file = ONE;
	} else if(file_size<UINT16_MAX) {
		o_file = TWO;
	} else if(file_size<UINT32_MAX) {
		o_file = FOUR;
	}	
	fputc(o_file, fout);
	for(j=8; j <= 8*o_file ;j+=8){
	fs = file_size >> ((8*o_file) - j);
	fputc(fs, fout);
	}
	fwrite(enc_out_begin,1, real_size, fout);
}
void freetree(struct tnode* rt)
{
	if(rt==NULL)
		return;
	freetree(rt->right);
	freetree(rt->left);
	free(rt);
}

/*
    @function main
*/
int main(int argc, char** argv)
{
    struct tnode* p=NULL;
    struct tnode* lc,*rc;
	int i=0;
    FILE* input = NULL;
	const char *OUT_FILE = "encoded.txt";
	if(!setjmp(err)){
		goto START;
	} else {
		printf("Not enough resources to proceed...\n");
		goto CLEAR;
	}
	START:
	if(argc!=2){
		printf("Please pass input file as a parameter.\n");
		return 0;
	} 
	input = fopen(*(argv+1),"r");
    /*number of characters*/
	calc_freq(input);
	FILE* fout=NULL;
	/*zero out code*/
	memset(code,0,sizeof(code));
    /*initialize with freq*/
    int nc = 0;
        while(1) {
            if(nc>255){
                break;
            }
            if(*(freq+nc) !=0) {
                pq_insert(talloc(nc,*(freq+nc)));
            }
            nc++;
        }
    /*build tree*/
    for(i=0;i<NCHAR-1;i++)
    {
        lc=pq_pop();
        rc=pq_pop();
        /*create parent*/
        p=talloc(0,lc->freq+rc->freq);
        /*set parent link*/
        lc->parent=rc->parent=p;
        /*set child link*/
        p->right=rc; p->left=lc;
		/*make it non-leaf*/
		p->isleaf=0;
        /*add the new node to the queue*/
        pq_insert(p);
    }
    /*get root*/
    root=pq_pop();
	/*build code*/
	generate_code(root,0);
	fout=fopen(OUT_FILE,"w");
	encode(input_data,fout);
	fclose(fout);
	CLEAR:
	if(input_data!=NULL){
		free(input_data);
	}
	freetree(root);
}	
