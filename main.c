#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "BPlusTree.h"

#define true 1
#define false 0
#define MAX_KEY 2500000000
#define BILLION 1000000000L
// file
char input_file[100];
char input_file2[100];
char output_file[100];
char *buffer,*buffer2;
int fsize;
// record
int new_key, new_pos, new_len;
char new_st[12];
// data
const int TotalRecords = 2500000000;
int validRecords;
// test
int keys[250000000], key_num;

/**
 * Read_Buffer(char *input_file) -> buffer
 */
inline char *Read_Buffer(char *buffer ,char *input_file) {
    FILE* fin = fopen(input_file, "rb");
    if (fin == NULL) {
        fputs("File Error\n", stderr);
        exit(1);
    }
    // obtain file size
    fseek(fin, 0, SEEK_END);
    fsize = ftell(fin);
    rewind(fin);
    // allocate memory
    buffer = (char*)malloc(sizeof(char) * fsize);
    if (buffer == NULL) {
        fputs("Memory Error\n", stderr);
        exit(2);
    }
    // read to buffer[]
    fread(buffer, 1, fsize, fin);
    fclose(fin);
    return buffer;
}

/** Read and insert records into B+tree */
inline void Read_Data_And_Insert(char* buffer) {
    int rid = 0;
    int cur = 0;
    while (1) {
        while (cur < fsize && !('0' <= buffer[cur] && buffer[cur] <= '9')) cur++; // end of file
        if (cur == fsize) break;

        rid++;
      //  if (rid % 100000 == 0) printf("now inserting the %d th record ..\n", rid);
        new_key = 0;
        new_pos = cur;
        while (buffer[cur] != ' ' && buffer[cur] != '\t') {
            new_key = new_key * 10 + (buffer[cur] - '0');
            cur++;
        }
        cur++;
        new_len = 0;
        while (buffer[cur] == ' ') cur++; // util meet a character
        while (cur < fsize && buffer[cur] != '\n') {
            new_st[new_len++] = buffer[cur++];
        }
        new_st[new_len] = '\0';
        char* value = (char*)malloc(sizeof(char) * new_len);
        strcpy(value, new_st);
        keys[key_num++] = new_key;
        if (BPlusTree_Insert(new_key, new_pos, value) == true) validRecords++; // for "ex-data.txt", valid = 9950138
    }
    free(buffer);
    buffer = NULL;
}

inline long long unsigned int Read_Data_And_Search(char* buffer) {
    int rid = 0;
    int cur = 0;
    int ret =0;
    int count = 0;
    long long unsigned int diff_time,total;
    struct timespec start,end;
    while (1) {
        while (cur < fsize && !('0' <= buffer[cur] && buffer[cur] <= '9')) cur++; // end of file
        if (cur == fsize) break;

        rid++;
        new_key = 0;
        new_pos = cur;
        while (buffer[cur] != ' ' && buffer[cur] != '\t') {
            new_key = new_key * 10 + (buffer[cur] - '0');
            cur++;
        }
        cur++;
        new_len = 0;
        while (buffer[cur] == ' ') cur++; // util meet a character
        while (cur < fsize && buffer[cur] != '\n') {
            new_st[new_len++] = buffer[cur++];
        }
        clock_gettime(CLOCK_REALTIME,&start);
        ret = BPlusTree_Find(new_key);
        clock_gettime(CLOCK_REALTIME,&end);
        diff_time = BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
        total += diff_time;
        if(ret == -1)
        {
          //  printf("new_key: %d\n",new_key);
            count++;
            continue;
        }

    }
   // printf("Count : %d\n",count);
    free(buffer);
    buffer = NULL;
    return total;
}
inline long long unsigned int Read_Data_And_Delete(char* buffer) {
    int rid = 0;
    int cur = 0;
    int ret =0;
    int count = 0;
    long long unsigned int diff_time,total;
    struct timespec start,end;
    while (1) {
        while (cur < fsize && !('0' <= buffer[cur] && buffer[cur] <= '9')) cur++; // end of file
        if (cur == fsize) break;

        rid++;
        new_key = 0;
        new_pos = cur;
        while (buffer[cur] != ' ' && buffer[cur] != '\t') {
            new_key = new_key * 10 + (buffer[cur] - '0');
            cur++;
        }
        cur++;
        new_len = 0;
        while (buffer[cur] == ' ') cur++; // util meet a character
        while (cur < fsize && buffer[cur] != '\n') {
            new_st[new_len++] = buffer[cur++];
        }
      //  printf("----new_key : %d------\n",new_key);
        clock_gettime(CLOCK_REALTIME,&start);
        ret = BPlusTree_Delete(new_key);
        clock_gettime(CLOCK_REALTIME,&end);
        diff_time = BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
        total += diff_time;
       /* 
        if(count ==127)
        {
            
            return;
        }*/
        
        
            if(ret == -2){

            count++;
            continue;
        }




    }
    free(buffer);
    buffer = NULL;
    printf("Count: %d\n",count);
    return total;
}

/** Modify (key, value) on data file */
int File_Modify(int pos, int key, char *value) {
    int old_key, old_len, len = strlen(value), i;
    char old_value[12];

    FILE* file = fopen(input_file, "r+");
    fseek(file, pos, SEEK_SET);
    fscanf(file, "%d %s", &old_key, old_value); // read old
    old_len = strlen(old_value);

    if (len > old_len) return false; // conflict
    // rewrite
    fseek(file, pos - ftell(file), SEEK_CUR); // return to previous position
    fprintf(file, "%d\t%s", key, value);
    for (i = len; i < old_len; i++) fprintf(file, " "); // space fill blanks
    fclose(file);
    return true;
}

/** Delete (key, value) on data file */
void File_Delete(int pos) {
    int old_key, i;
    char old_value[12];

    FILE* file = fopen(input_file, "r+");
    fseek(file, pos, SEEK_SET);
    fscanf(file, "%d %s", &old_key, old_value); // read old
    int end_pos = ftell(file);

    printf("%d %d\n", pos, end_pos);
    // rewrite
    fseek(file, pos - end_pos, SEEK_CUR); // return to previous position
    for (i = pos; i < end_pos; i++) fprintf(file, " "); // space fill blanks
    fclose(file);
}

/** Insert (key, value) on data file */
int File_Insert(int new_key, char* new_st) {
    FILE* file = fopen(input_file, "r+");
    fseek(file, 0, SEEK_END);
    int new_pos = ftell(file);

    fprintf(file, "%d %s\n", new_key, new_st);
    fclose(file);
    return new_pos;
}

/** Show Help */
void ShowHelp() {
    printf("\nType your operation:\n");
    printf("  0) Test Initialize\n");
    printf("  1) Set Depth\n");
    printf("  2) Set MaxChildNumber\n");
    printf("  3) Build Tree\n");
    printf("  4) Query on a key\n");
    printf("  5) Query on keys of range [l, r]\n");
    printf("  6) Modify value on a key\n");
    printf("  7) Delete value on a key\n");
    printf("  8) Insert new record\n");
    printf("  9) Debug\n");
    printf("  10) Quit\n");
}

void MainLoop() {
    double start_time, end_time;
    struct timespec start,end;
    long long unsigned int diff_time;
    int built1 = false;
    int built2 = false;
                        // args
                      built2 = false;
                        validRecords = 0;
    // B+tree initialize
    BPlusTree_Init();
    while (1) {
        ShowHelp();
        int request;
        scanf("%d", &request);
        switch (request) {
            case 0: {
                        long long unsigned int total = 0;
                        if (built2 == true) {
                            printf("You have built the B+tree\n");
                            break;
                        }
                        buffer2 = Read_Buffer(buffer2,input_file2);
                        built2 = true;
                        clock_gettime(CLOCK_REALTIME,&start);
                        Read_Data_And_Insert(buffer2);
                        clock_gettime(CLOCK_REALTIME,&end);
                        diff_time = BILLION * (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec);
                        total += diff_time;

                       // BPlusTree_Print();

                        printf("Valid Records inserted on B+tree = %d\n", validRecords);
                        printf("Total number of B+tree nodes = %d\n", BPlusTree_GetTotalNodes());
                        printf("Build B+tree costs %llu ms\n",total /1000000);
                        printf("Total number of split_count  = %d\n", BPlusTree_GetSplitCount());
                        printf("Node size %lu byte\n",sizeof(struct BPlusTreeNode));
 
                        break;
                    }
            case 1: {
                        // Set Depth
                        printf("input depth: ");
                        int depth;
                        scanf("%d", &depth);
                        int maxCh = 2;
                        while (1) {
                            int leaves = 1, i;
                            for (i = 0; i < depth; i++) {
                                leaves *= maxCh;
                                if (leaves > TotalRecords) break;
                            }
                            if (leaves > TotalRecords) break;
                            maxCh++;
                        }
                        printf("Desired depth = %d, calculated maxChildNumber = %d\n", depth, maxCh);
                        BPlusTree_SetMaxChildNumber(maxCh);
                        break;
                    }
            case 2: {
                        // Set MaxChildNumber
                        printf("input MaxChildNumber: ");
                        int maxCh;
                        scanf("%d", &maxCh);
                        BPlusTree_SetMaxChildNumber(maxCh);
                        break;
                    }
            case 3: {
                        long long unsigned int total = 0 ;
                        // Build B+tree
                        if (built1 == true) {
                            printf("You have built the B+tree\n");
                            break;
                        }
                        buffer = Read_Buffer(buffer,input_file);
                        built1 = true;
                        clock_gettime(CLOCK_REALTIME,&start);
                        Read_Data_And_Insert(buffer);
                        clock_gettime(CLOCK_REALTIME,&end);
                        diff_time = BILLION * (end.tv_sec - start.tv_sec) + end.tv_nsec - start.tv_nsec;
                        total += diff_time;

                        printf("Valid Records inserted on B+tree = %d\n", validRecords);
                        printf("Total number of B+tree nodes = %d\n", BPlusTree_GetTotalNodes());
                        printf("Build B+tree costs %llu ms\n",total /1000000);
                        printf("Total number of split_count  = %d\n", BPlusTree_GetSplitCount());
                        //        BPlusTree_Print();
                        break;
                    }
            case 4: {
                        long long unsigned int total = 0;
                        buffer2 = Read_Buffer(buffer2,input_file2);
                        total = Read_Data_And_Search(buffer2);

                        printf(" Total time %llu ms\n", total / CLOCKS_PER_SEC);
                        break;
                    }
            case 5: {
                        // Query on a range [l, r]
                        printf("input range [l, r]: ");
                        int l, r;
                        scanf("%d %d", &l, &r);
                        if (l > r) {
                            printf("input illegal\n");
                            break;
                        }
                        start_time = clock();
                        BPlusTree_Query_Range(l, r);
                        end_time = clock();
                        printf("Query on a range, costs %lf s\n", (end_time - start_time) / CLOCKS_PER_SEC);
                        break;
                    }
            case 7:{
                       long long unsigned int total = 0;

                       buffer2 = Read_Buffer(buffer2,input_file2);
                       total = Read_Data_And_Delete(buffer2);
            //            BPlusTree_Print();
                       printf("Total time costs %llu ms\n",total /1000000);
                       break;

                   }/*
                       {
                   // Modify value on a key
                   case 9:{
                   int num,j,value;
                   double total,temp;
                   char s[] = "dkdka";
                   value = 4;
                   total = 0;
                   temp = 0;
                   for(j=1; j<=4; j++)
                   {
                   start_time = clock();
                   BPlusTree_Insert(j,0,s);
                   end_time = clock();
                   total += end_time - start_time;

                   validRecords++;
                   }
                   BPlusTree_Print();
                   printf("Valid Records inserted on B+tree = %d\n", validRecords);
                   printf("Total number of B+tree nodes = %d\n", BPlusTree_GetTotalNodes());
                   printf("Build B+tree costs %lf s\n", (end_time - start_time) / CLOCKS_PER_SEC);
                   printf("Total time %lf s\n",total/ CLOCKS_PER_SEC);
                   printf("Total number of split_count : %d\n",BPlusTree_GetSplitCount());
                   break;

                   }
                   */
            case 10: return;
            default: break;

        }
        }

    }
int main(int argc, char *argv[]) {
	// set input_file, output_file
	strcpy(input_file, "data/small-data.txt");
	strcpy(input_file2, "data/rand-data.txt");
	if (argc == 2) strcpy(input_file, argv[1]);

	// MainLoop (for presentation)
	MainLoop();
	
	//build_test();
	//query_key_test();
	//query_range_test();
	//modify_test();
	//delete_test();
	return 0;
}


