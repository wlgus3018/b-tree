#ifndef __BPlusTree_H__
#define __BPlusTree_H__

#define MAX_CHILD_NUMBER 5

typedef struct BPlusTreeNode {
	int isRoot, isLeaf;
	int key_num;
	int key[MAX_CHILD_NUMBER];
	void* child[MAX_CHILD_NUMBER+1];
	struct BPlusTreeNode* father;
	struct BPlusTreeNode* next;
	struct BPlusTreeNode* last;
        int unused[17];
} BPlusTreeNode;

extern void BPlusTree_SetMaxChildNumber(int);
extern void BPlusTree_Init();
extern void BPlusTree_Destroy();
extern int BPlusTree_Insert(int, int, void*);
extern int BPlusTree_GetTotalNodes();
extern void BPlusTree_Query_Key(int);
extern void BPlusTree_Query_Range(int, int);
extern void BPlusTree_Modify(int, void*);
extern int BPlusTree_Delete(int);
extern int BPlusTree_GetSplitCount();
#endif
