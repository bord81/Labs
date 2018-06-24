#include "common.h"

struct tnode
{
    struct  tnode* left; /*used when in tree*/
    struct  tnode*right; /*used when in tree*/  
    int     isleaf;
    unsigned char     symbol;
};

/*global variables*/
struct tnode* root=NULL; /*tree of symbols*/
int     code_begin = 0;

/*
    @function   talloc
    @desc       allocates new node 
*/
struct tnode* talloc()
{
    struct tnode* p=(struct tnode*)malloc(sizeof(struct tnode));
    if(p!=NULL)
    {
        p->left=p->right=NULL;
        p->symbol=0;
		p->isleaf=0;
    }
    return p;
}

/*
    @function build_tree
    @desc     builds the symbol tree given the list of symbols and code.h
	NOTE: alters the global variable root that has already been allocated in main
*/
void build_tree(FILE* fp)
{
	unsigned char	symbol;
	unsigned char c;
	unsigned char	len;
       	int	i;
	uint32_t	strcode;
	struct	tnode* curr=NULL;


        int total_chars=0; 
        total_chars = fgetc(fp);
		curr = root;
	while(!feof(fp))
	{
        if(total_chars==0){
            break;
        }
        total_chars--;
	strcode=0;
		symbol = fgetc(fp);
		for(int j=3;j>-1;--j){
		c = fgetc(fp); 
		strcode |= (c << (8*j));
		}
		len = fgetc(fp);

			code_begin += 6;
	    
		for(i=len-1;i>=0;--i)
		{
			if((strcode >> i) & 1) {
				if(root->right==NULL){
				root->right=talloc();
		
				}
				root=root->right;
			} else {
				if(root->left==NULL){
				root->left=talloc();
		
				} 
				root=root->left;
			}
		}
		root->isleaf=1;
		root->symbol=symbol;
		root = curr;
	}
}

/*
	function decode
*/
void decode(FILE* fin,FILE* fout)
{
	unsigned char c;
	int i = 0;
	int j = 7;
	int k = 1;
	int code_pointer = 0;
	o_fsize ofs = EIGHT;
	int64_t file_size = 0;
	struct tnode* curr=root;

	while(!feof(fin))
	{
		c = fgetc(fin);
		if(code_pointer <= code_begin) {
			code_pointer++;
			continue;
		}
		if(k>0){
			--k;
			ofs = c;
			j = ofs - 1;
			continue;
		}
		if(j>-1) {
			file_size |= (c << (8*j));
			--j;
			continue;
		}
		
		for (i=7; i>=0; --i) {
			if(!file_size){
				break;
			}
		if((c >> i) & 1){
			curr=curr->right;	
			if(curr->isleaf==1){
//				printf("%c",curr->symbol);
				fputc(curr->symbol,fout);
				curr=root;			
				--file_size;
			}	
											
		} else {
			curr=curr->left;
			if(curr->isleaf==1){
//				printf("%c",curr->symbol);
				fputc(curr->symbol,fout);
				curr=root;		
				--file_size;	
			}	
		}
	}
	}
}
/*
	@function freetree
	@desc	  cleans up resources for tree
*/

void freetree(struct tnode* root)
{
	if(root==NULL)
		return;
	freetree(root->right);
	freetree(root->left);
	free(root);
}
int main()
{
	const char* IN_FILE="encoded.txt";
	const char* OUT_FILE="decoded.txt";
	FILE* fout = NULL;
	FILE* fin = NULL;
	/*allocate root*/
	root=talloc();
	fin=fopen(IN_FILE,"r");
	/*build tree*/
	build_tree(fin);
	fclose(fin);
	/*decode*/
	fin=fopen(IN_FILE,"r");
	fout=fopen(OUT_FILE,"w");
	decode(fin,fout);
	fclose(fin);
	fclose(fout);
	/*cleanup*/
	freetree(root);
	return 0;
}

