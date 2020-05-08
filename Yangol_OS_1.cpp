#include <iostream>
#include <string.h>
// лабораторна робота студента групи ІС-71 Янголя В.Є.
using namespace std;

#define USED_1_CONSTANT 1
#define IF_NOT_FOUND 2

static char ALL_BUFFER[209776640];
static unsigned long long AVAILABLE_AT_THE_MOMENT = 209776640;
static char* pools[2048];
static char* blocks[2048];
static unsigned int BLOCKS_COUNT = 0;
static unsigned int POOLS_COUNT = 1;
static unsigned int pools_size[2048];
static unsigned int block_size[2048];
static unsigned int RIGHT_BLOCK;
static int ARS_ALLOC_ERR = NULL;

void mem_init(void);
void mem_init(void){
	pools[0] = ALL_BUFFER;
	pools_size[0] = AVAILABLE_AT_THE_MOMENT;
}
char* mem_alloc(unsigned long Size);
char* mem_alloc(unsigned long Size){
	unsigned long int i, k;
	char* pool;
	if (Size > AVAILABLE_AT_THE_MOMENT){
		ARS_ALLOC_ERR = USED_1_CONSTANT;
		return 0;
	}
	pool = 0;

	for (i = 0; i < POOLS_COUNT; ++i)
		if (Size <= pools_size[i]){
			pool = pools[i];
			k = i;
			break;
		}
	if (!pool){
		ARS_ALLOC_ERR = USED_1_CONSTANT;
		return 0;
	}
	blocks[BLOCKS_COUNT] = pool;
	block_size[BLOCKS_COUNT] = Size;
	++BLOCKS_COUNT;
	++RIGHT_BLOCK;
	pools[k] = (char*)(pool + Size + 1);            
	pools_size[k] = pools_size[k] - Size;
	AVAILABLE_AT_THE_MOMENT -= Size;
	return pool;
}

char* mem_realloc(char*, unsigned long);
char* mem_realloc(char* pool, unsigned long Size){
	unsigned long int i, k;
	char* newpool;
	if (Size > AVAILABLE_AT_THE_MOMENT){
		ARS_ALLOC_ERR = USED_1_CONSTANT;
		return 0;
	}
	for (i = 0; i < POOLS_COUNT; ++i)
		if (Size <= pools_size[i]){
			pool = pools[i];
			k = i;
			break;
		}
	newpool = pool;
	if (!newpool){
		ARS_ALLOC_ERR = USED_1_CONSTANT;
		return 0;
	}

	blocks[BLOCKS_COUNT] = newpool;
	block_size[BLOCKS_COUNT] = Size;
	++BLOCKS_COUNT;
	++RIGHT_BLOCK;
	pools[k] = (char*)(newpool + Size + 1);
	pools_size[k] = pools_size[k] - Size;

	AVAILABLE_AT_THE_MOMENT -= Size;
	return newpool;
}


int mem_frag(void);
int mem_frag(void){
	unsigned int i, k;
	char* p = ALL_BUFFER;
	char* t, * tmp;
	for (i = 0; i < RIGHT_BLOCK; ++i){
		t = blocks[i];
		if (t == ALL_BUFFER){
			p = (char*)(blocks[i] + block_size[i] + 1);
			continue;
		}
		tmp = p;
		for (k = 0, t = blocks[i]; k < block_size[i]; ++k)
			*p++ = *t++;
		blocks[i] = tmp;
	}
	POOLS_COUNT = 1;
	pools[0] = p;
	AVAILABLE_AT_THE_MOMENT = 209776640 - (unsigned long)(p - ALL_BUFFER);
	pools_size[0] = AVAILABLE_AT_THE_MOMENT;
	RIGHT_BLOCK = 0;
	return 0;
}




void mem_free(char*);
void mem_free(char* block){
	unsigned int i, k = 0;
	char* p = 0;
	for (i = 0; i < RIGHT_BLOCK; ++i)
		if (block == blocks[i]){
			p = blocks[i];
			k = i;
			break;
		}

	if (!p){
		ARS_ALLOC_ERR = IF_NOT_FOUND;
	}

	blocks[k] = 0;
	--BLOCKS_COUNT;
	pools[POOLS_COUNT] = block;
	pools_size[POOLS_COUNT] = block_size[k];
	++POOLS_COUNT;
	AVAILABLE_AT_THE_MOMENT += block_size[k];

}




int main(){
	mem_init();
	char* pool = mem_alloc(1);
	cout << "|---------------------------|" << endl;
	cout << "|" << "mem_alloc_fun_result:      | \n";
	cout << "|" <<&pool << "           |"<< endl;
	cout << "|---------------------------|" << endl;
	char* newpool = mem_realloc(pool, 6);
	cout << "|mem_realloc_fun_result:    |\n";
	cout << "|" << &newpool << "           |" << endl;
	cout << "|---------------------------|" << endl;
	mem_frag();
	mem_free(pool);
	mem_free(newpool);
	system("pause");
	return 0;
}

