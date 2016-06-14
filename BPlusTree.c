#include "BPlusTree.h"
#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0

struct BPlusTreeNode* Root;
int split_count = 0;
int MaxChildNumber = 29;
int TotalNodes;

int QueryAnsNum;

/** Create a new B+tree Node */
BPlusTreeNode* New_BPlusTreeNode() {
	struct BPlusTreeNode* p = (struct BPlusTreeNode*)malloc(sizeof(struct BPlusTreeNode));
	p->isRoot = false;
	p->isLeaf = false;
	p->key_num = 0;
	p->child[0] = NULL;
	p->father = NULL;
	p->next = NULL;
	p->last = NULL;
	TotalNodes++;
	return p;
}

/** Binary search to find the biggest child l that Cur->key[l] <= key */
inline int Binary_Search(BPlusTreeNode* Cur, int key) {
	int l = 0, r = Cur->key_num;
	if (key < Cur->key[l]) return l;
	if (Cur->key[r - 1] <= key) return r - 1;
	while (l < r - 1) {
		int mid = (l + r) >> 1;
                
                
		if (Cur->key[mid] > key)
			r = mid;
		else
			l = mid;
	}
	return l;
}

/**
 * Cur(MaxChildNumber) split into two part:
 *	(1) Cur(0 .. Mid - 1) with original key
 *	(2) Temp(Mid .. MaxChildNumber) with key[Mid]
 * where Mid = MaxChildNumber / 2
 * Note that only when Split() is called, a new Node is created
 */
void Insert(BPlusTreeNode*, int, int, void*);
void Split(BPlusTreeNode* Cur) {
	// copy Cur(Mid .. MaxChildNumber) -> Temp(0 .. Temp->key_num)
	BPlusTreeNode* Temp = New_BPlusTreeNode();
        split_count++;
	int Mid = MaxChildNumber >> 1;
	Temp->isLeaf = Cur->isLeaf; // Temp's depth == Cur's depth
	int i;
        int M_key = Cur->key[Mid];

        if(Cur->isLeaf == false)
        {
        //    printf("hi!\n");
            BPlusTreeNode* ch = New_BPlusTreeNode();
            for (i = 0; i < Mid; i++) {
                Temp->child[i] = Cur->child[i+Mid+1];
                Temp->key[i] = Cur->key[i+Mid+1];
                ch = Temp->child[i];
                ch->father = Temp;
            }
            Temp->child[i] = Cur->child[i+Mid+1];
            Cur->key_num = Mid;
            Temp->key_num =Mid;
          //  Cur->next = Temp;
            ch =Temp->child[i];
          //
            ch ->father = Temp;

        }
        else
        {
            for (i = 0; i <= Mid; i++) {
                Temp->child[i] = Cur->child[i+Mid];
                Temp->key[i] = Cur->key[i+Mid];
            }
            Cur->key_num = Mid;
            Temp->key_num = Mid+1;
          //  Cur->next= Temp;
        }
        // Insert Temp
        if (Cur->isRoot) {
            // Create a new Root, the depth of Tree is increased
            Root = New_BPlusTreeNode();
            Root->key_num = 1;
            Root->isRoot = true;
            Root->key[0] = M_key;
            Root->child[0] = Cur;
            Root->child[1] = Temp;
            Cur->father = Temp->father = Root;
            Cur->isRoot = false;
            Cur->next = Temp;
            Temp->last = Cur;
            //printf("Root: %d\n",Root->key[0]);
        } else {
            // Try to insert Temp to Cur->father
            Temp->father = Cur->father;
           
           // printf("Cur->key[Mid] : %d\n",Cur->key[Mid]);
           // printf("Temp->key[0] : %d\n",Temp->key[0]);
            Insert(Cur->father, M_key, -1, (void*)Temp);
        }
}

void Delete(BPlusTreeNode*, int);
/** Insert (key, value) into Cur, if Cur is full, then split it to fit the definition of B+tree */
void Insert(BPlusTreeNode* Cur, int key, int pos, void* value) {
    int i, ins;


    if (key < Cur->key[0]) ins = 0; else ins = Binary_Search(Cur, key) + 1;
    if(Cur->isLeaf)
    {
        for (i = Cur->key_num; i >ins; i--) {
            Cur->key[i] = Cur->key[i-1];
            Cur->child[i] = Cur->child[i-1];
        }
        Cur->key_num++;
        Cur->key[ins] = key;
        Cur->child[ins] = value;
       //     printf("key: %d\n",key);
         //   printf("key[0]: %d\n",Cur->key[0]);
          //printf("ins : %d\n",ins);
    }
    else
    {

        //  printf("ins : %d\n",ins);
          //ins--;
          /*
          if(Cur->key_num == 1)
          {
              ins--;
          }*/
         
  //      printf("here\n");
    //    printf("Cur->key_num: %d\n",Cur->key_num);
      //  printf("key: %d\n",key);
        for (i = Cur->key_num; i >ins; i--) {
            Cur->key[i] = Cur->key[i-1];
            Cur->child[i+1] = Cur->child[i];
        }
        Cur->key_num++;
        Cur->key[ins] = key;
        Cur->child[ins+1] = value;

    }

	if (Cur->isLeaf == false) {
            BPlusTreeNode *temp,*prev;
            if(ins == 0)
            {
                temp = value;
                prev = Cur->child[0];
                temp->next = prev->next;
                prev->next = temp;
            }
            else
            {
                temp = value;
                prev = Cur->child[ins];
             //   printf("Non-leaf ins:%d\n",ins);
               // printf("prev: %p\n",Cur->child[ins]);
                temp->next = prev->next;
                prev->next = temp;
            }

	}
	if (Cur->key_num == MaxChildNumber) // children is full
        {
            //printf("Split!!\n");
		Split(Cur);
        }
}



void Merge_Node(BPlusTreeNode *Cur,int key,void *value)
{
    int i, ins,u;
    int t;
    //    printf("ins : %d\n",ins);
    if (key < Cur->key[0]) ins = 0; else ins = Binary_Search(Cur, key) + 1;

    if(Cur->isLeaf)
    {
        for (i = Cur->key_num; i >ins; i--) {
            Cur->key[i] = Cur->key[i-1];
            Cur->child[i] = Cur->child[i-1];
        }
        Cur->key_num++;
        Cur->key[ins] = key;
        Cur->child[ins] = value;
        //     printf("key: %d\n",key);
        //   printf("key[0]: %d\n",Cur->key[0]);
        //printf("ins : %d\n",ins);
    }
    else
    {

        //  printf("ins : %d\n",ins);
        //ins--;
        /*
           if(Cur->key_num == 1)
           {
           ins--;
           }*/

        //      printf("here\n");
        //    printf("Cur->key_num: %d\n",Cur->key_num);
        //  printf("key: %d\n",key);
        
        BPlusTreeNode *child = Cur->child[ins];
        if(ins == 0 && child->key[0] >= key)
        {
            for (i = Cur->key_num; i >ins; i--) {
                Cur->key[i] = Cur->key[i-1];
                Cur->child[i+1] = Cur->child[i];
            }
            Cur->child[i+1] = Cur->child[i];
            Cur->child[0] = value; 
            Cur->key_num++;
            Cur->key[ins] = key;
        }

        else
        {
        for (i = Cur->key_num; i >ins; i--) {
            Cur->key[i] = Cur->key[i-1];
            Cur->child[i+1] = Cur->child[i];
        }
      //  printf("i :%d\n",i);
        Cur->key_num++;
        Cur->key[ins] = key;
        Cur->child[ins+1] = value;
        }
    }









}
unsigned int Finding_Moving_Child(BPlusTreeNode *father, BPlusTreeNode *Cur)
{
    int index;


    index = Binary_Search(father,Cur->key[0]);
    

 //   printf("Finding index : %d\n",index;
  //  printf("Cur->key : %d\n",Cur->key[Cur->slot_array[1]]);

   // printf("temp:%d\n",temp->key[temp->slot_array[1]]);

   
    while(1)
    {
        
        if(index == 0)
        {
            return father->key[0];
        }/*
        if(temp->key[temp->slot_array[1]] == father->key[father->slot_array[index]])
        {
       // printf("hi");
            return father->key[father->slot_array[index]];
        }*/
        else
        {
            return father->key[index];
        }
        

        //index--;
        

    }
}

void Re_Delete(BPlusTreeNode *Cur, int key)
{
    int ins;
    int i;
    //WBPlusTreeNode *temp = New_WBPlusTreeNode();


    ins = Binary_Search(Cur,key);
    //printf("Cal Redelete\n");
    
    /*for(j = 0; j<Cur->key_num; j++)
    {
        printf("Cur->key[%d] : %d\n",j,Cur->key[Cur->slot_array[j+1]]);
    }*/
   // printf("Cur->key_num : %d\n",Cur->key_num);
   // printf("ins: %d\n",ins);
   // printf("cur->key[0]: %d\n",Cur->key[0]);
   // printf("key: %d\n",key);

    if(Cur->isLeaf== false)
    {
                
        for(i=ins; i<Cur->key_num-1; i++)
        {
          //  printf("hihihihi\n");
            Cur->key[i] = Cur->key[i+1];
            Cur->child[i+1] = Cur->child[i+2];
        }
    Cur->key_num = Cur->key_num - 1;

    }
    else
    {
    
    for(i=ins; i<Cur->key_num-1; i++)
    {
        Cur->key[i] = Cur->key[i+1];
        //Cur->child[i] = Cur->child[i+2];
    }
    Cur->key_num = Cur->key_num - 1;
    }

    /* clear slot_aray == 0
       if(Cur->key_num == 0)
       {
       Cur->slot_array[i-1] = 0;

       }*/
    /*
    printf("================================================\n");
    printf("Cur->key[0]: %d\n",Cur->key[0]);
         printf("Cur->child[0] : %p\n",Cur->child[0]);
         printf("Cur->child[1] : %p\n",Cur->child[1]);
    printf("================================================\n");
    */
}
BPlusTreeNode* Find_Child(BPlusTreeNode* Cur, int key){

    int index = Binary_Search(Cur,key);

    if(index == 0)
    {
        //printf("Child 0!\n");
        return Cur->child[0];
    }
    else
    {
        //printf("Child not 0!\n");
        return Cur->child[index+1];
    }

}
void Delete(BPlusTreeNode *Cur, int key){
   
    int ins;
    int i;
   BPlusTreeNode *father;

    
    ins = Binary_Search(Cur,key);


    //printf("what is ins: %d\n",ins);
    /*:L
    if(Cur->isRoot)
    {
        printf("Root!!\n");
    }*/
     for(i = ins; i<Cur->key_num-1; i++)
    {
        Cur->key[i] = Cur->key[i+1];
        Cur->child[i] = Cur->child[i+1];
    }
  
  Cur->key_num = Cur->key_num - 1;
    //Cur->slot_array[i+1] = 0;
        
    if(ins == 0 && !Cur->isRoot){
//        printf("Normal ins : %d\n",ins);

        BPlusTreeNode *temp;
        temp = Cur;
                
        while(1){
            int i = Binary_Search(temp, key);
            
           if(temp->isRoot == true)
            {
                if(temp->key[i] == key)
                {
                    temp->key[i] = Cur->key[0];
                //    printf("Cur->key[Cur->slot_array[1]] : %d\n",Cur->key[Cur->slot_array[1]]);
                 //   printf("root ins : %d\n",ins);

                }
                break;
            }
            if(temp->key[i] == key)
            {
               // printf("temp->key[0] : %d\n",temp->key[temp->slot_array[1]]);
                temp->key[i] = Cur->key[0];
               // printf("temp->key[Cur->slot_array[1]] : %d\n",temp->key[temp->slot_array[i]]);
               // printf("Second ins : %d\n",ins);

                temp = temp->father;
                continue;
            }
                temp = temp->father;
        }
    }


     BPlusTreeNode *temp = Cur;
    if(temp->key_num  <= 1 && !temp->isRoot)
    {

       // printf("key!!\n");
        //printf("root : %p\n",Root);
        //printf("Root->key_num: %d\n",Root->key_num);
        //printf("temp : %p\n",temp);
        //printf("temp->key_num: %d\n",temp->key_num);
       // Debug_Print(Root);
       // Debug_Print(temp);
       // WBPlusTree_Print();
        if(Redistribute(temp) == false)
        {
         //   printf("Redistibute fail!\n"); 
          //  printf("Merge!!\n");
            Merge(temp);
        }
    }
/*Checking the last leaf deletion*/
    father = Cur->father;

    

    if(father != NULL)
    {
        while(1)
        {

            Cur = Cur->father;


            if(Cur->key_num <=1 && !Cur->isRoot)
            {
           //     printf("Upper!\n");
                if(Redistribute(Cur) == false)
                {
             //       printf("Non-leaf merge!!\n");
                    Merge(Cur);
                }

            }
            if(Cur->isRoot)
            {
                break;
            }

        }
    }
    if(father != NULL && father->isRoot && father->key_num == 0)
    {
       // printf("Free Root!\n");
       // printf("shit\n");
        Root = father->child[0];
        Root->isRoot = true;
        Root->father = NULL;
        Root->isLeaf = true;
        free(father);
        return ;

    }
   // printf("***********************\n");
   // printf("Delete success\n");
   // printf("***********************\n");

    

}

int Merge(BPlusTreeNode *Cur)
{
    BPlusTreeNode *father = Cur->father;
    int j,temp,Move_key,Root_up_key,index;

    BPlusTreeNode *next,*prev,*t;

    if(Cur->key[0] < father->key[0])
    {
        index = 0;
    }
    else
    {
    index = Binary_Search(father,Cur->key[0])+1;
    }
   // printf("Merge index : %d\n",index);
    if(Cur->isLeaf == false)
    {
     //           printf("Non leaf merge!!\n");
        if(index == 0)
        {


                //printf("Non leaf merge!!\n");
                next = father->child[index+1];
                prev = NULL;

               // printf("hi\n");
               // printf("next: %d\n",next->key[next->slot_array[1]]);
                //printf("Cur->father :%d\n",father->key[father->slot_array[1]]);

                for(j=0; j < next->key_num; j++)
                {

                    Merge_Node(Cur,next->key[j],next->child[j+1]);
                    t = next->child[j+1];
                    t->father = Cur;
                }
                Move_key = Finding_Moving_Child(father,next);
                temp = Move_key;
                BPlusTreeNode *child = Find_Child(next,Move_key);
                child->father = Cur;

                Merge_Node(Cur,Move_key,child);

                
                Cur->next = next->next;
                
                //Debug_Print(Cur);
                /*Adjusting Cur child Postion in Non_leaf*/
                /*
                   Move_key = Finding_Moving_Child(father,Cur);
                   child = Find_Child(father,Move_key);
                   Merge_Node(father,Move_key,child);
                   Re_Delete(father,Move_key);
                   child->father = father;
                   */
                //Debug_Print(Cur);



                
                /*Remove original father key*/
                Re_Delete(father,temp);
                
                free(next);
                if(father->isRoot && father->key_num == 0)
                {
                    //printf("Free Root!\n");
                    //printf("Root : %p\n",Root);
                    //printf("father : %p\n",father);
                    BPlusTreeNode *t = father->child[0];
                    //printf("t->key[0]: %d\n",t->key[t->slot_array[1]]);
           
                 //   Debug_Print(Cur);
                    Root = t;
                    
                    Root->isRoot = true;
                    Root->father = NULL;
                    free(father);
                }

                return true;

        }

    else if(index == father->key_num)
    {
        //printf("hi3\n");
        //printf("Non leaf merge!!\n");
        if(index == 0)
        {
            prev = father->child[0];
        }
        else
        {
            prev = father->child[index-1];//prev
        }
        next = NULL;
        //printf("father key : %d\n",father->key[father->slot_array[1]]);
        //printf("prev :%d\n",prev->key[prev->slot_array[1]]);
        //Debug_Print(prev);
        //Debug_Print(Cur);

        //printf("what is t : %p\n",Cur->child[Cur->slot_array[1]+1]);

        Merge_Node(prev,Cur->key[0],Cur->child[1]);
       //  printf("Cur->child[0] : %p\n",Cur->child[0]);
       // printf("Cur->child[1] : %p\n",Cur->child[1]);

        t = Cur->child[1];
        t->father = prev;
     //   Debug_Print(prev);
       // Debug_Print(Cur);
        Move_key = Finding_Moving_Child(father,Cur);
        BPlusTreeNode *child = Find_Child(Cur,Move_key);
        child->father = prev;

       // printf("what is Move_key : %d\n",Move_key);
        //printf("child : %p\n",child);
        Merge_Node(prev,Move_key,child);
        Re_Delete(father,Move_key);
       
       // Debug_Print(prev);
       // Debug_Print(Cur);

        //sleep(1000);

        //  Re_Child(father,prev->key[prev->slot_array[1]],true);

        prev->next = Cur->next;
        free(Cur);
        if(father->isRoot && father->key_num == 0)
        {
          //  printf("Delete Root\n");
            Root = father->child[0];
            Root->isRoot = true;
            Root->father = NULL;
            free(father);
        }



        return true;
    }
        else
        {   //printf("hi2\n"); 
            //printf("Non leaf merge!!\n");
            if(index == 0)
            {
                prev = father->child[0];
            }
            else
            {
                prev = father->child[index-1];
            }

            next = father->child[index+1];

            //printf("prev :%d\n",prev->key[prev->slot_array[1]]);
            //printf("next :%d\n",next->key[next->slot_array[1]]);

/*
            if(prev->key_num <=2 &&prev->key_num < next->key_num)
            {
                printf("Come here impossible\n");
                sleep(100);
                               

               // if(prev->key_num == next->key_num || prev->key_num > next->key_num})i
               }*/
                //else
                { 
                    Merge_Node(prev,Cur->key[0],Cur->child[1]);
                   // printf("Cur->child[0] : %p\n",Cur->child[0]);
                   // printf("Cur->child[1] : %p\n",Cur->child[1]);

                    t = Cur->child[1];
                    t->father = prev;
                    //   Debug_Print(prev);
                    // Debug_Print(Cur);
                    Move_key = Finding_Moving_Child(father,Cur);
                    BPlusTreeNode *child = Find_Child(Cur,Move_key);
                    child->father = prev;

                    // printf("what is Move_key : %d\n",Move_key);
                    //printf("child : %p\n",child);
                    Merge_Node(prev,Move_key,child);
                    Re_Delete(father,Move_key);

                    // Debug_Print(prev);
                    // Debug_Print(Cur);

                    //sleep(1000);

                    //  Re_Child(father,prev->key[prev->slot_array[1]],true);

                    prev->next = Cur->next;
                    free(Cur);

                    if(father->isRoot && father->key_num == 0)
                    {
                        Root = father->child[0];
                        Root->isRoot = true;
                        Root->father = NULL;
                        free(father);
                    }
                    return true;
                }



        }

    }
    else{
/**********************/
       // printf("Normal Merge!\n");

    if(index == 0){

        next = father->child[index+1];

        //next = Cur->next;

        prev = NULL;

        //printf("hi\n");
        //printf("Normal Merge!\n");
        //printf("next: %d\n",next->key[0]);
        //printf("next: %p\n",next);
        // Insert(next,Cur->key[Cur->slot_array[1]],Cur->child[Cur->slot_array[1]]);
        // Delete(father,next->key[next->slot_array[1]]);


        //printf("Cur key:%d\n",Cur->key[Cur->slot_array[1]]);
        for(j=0; j < next->key_num; j++)
        {

            Merge_Node(Cur,next->key[j],next->child[j]);
           // t = next->child[j];
           // t->father = Cur->father;
        }
        /* 
           for(j=0; j < Cur->key_num; j++)
           {

           printf("Cur key+:%d\n",Cur->key[Cur->slot_array[j+1]]);
           }*/

        Re_Delete(father,next->key[0]);
        //memmove(newnode,Cur,sizeof(struct WBPlusTreeNode));
        //Insert(father,newnode->key[newnode->slot_array[1]],newnode);

        /*Adjusting the newnode child position in Non_leaf Node*/
        /*************************************************/

        //Merge_Node(father,Cur->key[0],Cur);
       // Debug_Print(Cur);
        //Re_Delete(father,Cur->key[0]);

        //Debug_Print(Cur);
        /*Re_Delete Becase We keep the key number and child number +1 */
        /************************************************/

        Cur->next = next->next;
        //


        // Re_Delete(father,next->key[next->slot_array[1]]);
        free(next);
        //  free(Cur);
        /* int i = slot_binary_search(father,next->key[next->slot_array[1]]);
           i--;
           if(father->key[father->slot_array[i]] == next->key[next->slot_array[1]])
           {

           }*/
       
       // printf("here\n");
        return true;


    }

    else if(index == father->key_num)
    {
        //printf("Normal Merge!\n");
  //      printf("hi3\n");
        if(index == 0)
        {
            prev = father->child[0];
        }
        else
        {
            prev = father->child[index-1];//prev
        }
        next = NULL;
        //printf("father key : %d\n",father->key[father->slot_array[1]]);
        //printf("prev :%d\n",prev->key[prev->slot_array[1]]);


        Merge_Node(prev,Cur->key[0],Cur->child[0]);
     //   t = Cur->child[0];
     //   t->father = prev->father;
        Re_Delete(father,Cur->key[0]);

//        Debug_Print(prev);

        /*Adjusting the newnode child position in Non_leaf Node*/
        /*************************************************/
      //  Merge_Node(father,prev->key[prev->slot_array[1]],prev);

       // Re_Delete(father,prev->key[prev->slot_array[1]]);
        /*Re_Delete Becase We keep the key number and child number +1 */
        /************************************************/




        //Re_Child(father,prev->key[prev->slot_array[1]],false);
        prev->next = Cur->next;
        free(Cur);
        return true;
    }
    else
    {   //printf("hi2\n"); 
       // printf("Normal Merge!\n");
        if(index == 0)
        {
            prev = father->child[0];
        }
        else
        {
            prev = father->child[index-1];
        }

        next = father->child[index+1];

        //printf("prev :%d\n",prev->key[prev->slot_array[1]]);
        //printf("next :%d\n",next->key[next->slot_array[1]]);
        //printf("prev->key_num : %d\n",prev->key_num);
        //printf("next->key_num : %d\n",next->key_num);


        if(prev->key_num <=2 && prev->key_num < next->key_num)
        {
       //     printf("Imossible\n");
         //   sleep(10);
            for(j=0; j < next->key_num; j++)
            {

                Merge_Node(Cur,next->key[j],next->child[j]);
                // t = next->child[j];
                // t->father = Cur->father;
            }
            /* 
               for(j=0; j < Cur->key_num; j++)
               {

               printf("Cur key+:%d\n",Cur->key[Cur->slot_array[j+1]]);
               }*/

            Re_Delete(father,next->key[0]);
            //memmove(newnode,Cur,sizeof(struct WBPlusTreeNode));
            //Insert(father,newnode->key[newnode->slot_array[1]],newnode);

            /*Adjusting the newnode child position in Non_leaf Node*/
            /*************************************************/

            //Merge_Node(father,Cur->key[0],Cur);
            // Debug_Print(Cur);
            //Re_Delete(father,Cur->key[0]);

            //Debug_Print(Cur);
            /*Re_Delete Becase We keep the key number and child number +1 */
            /************************************************/

            Cur->next = next->next;
            //


            // Re_Delete(father,next->key[next->slot_array[1]]);

            free(next);
        }
            //if(prev->key_num == next->key_num || prev->key_num > next->key_num)
            else
            {
                Merge_Node(prev,Cur->key[0],Cur->child[0]);
              //  t = Cur->child[0];
             //   t->father = prev->father;
                Re_Delete(father,Cur->key[0]);
                //Re_Child(father,Cur->key[Cur->slot_array[1]],false);
                prev->next = Cur->next;
                   free(Cur);
                return true;
            }

        


    }
    }

}


void replace(BPlusTreeNode *Cur,int key,unsigned char flag)
{
    int i;
    if(flag == 5)
    {
        /*Redistrubute for middle & next*/
        i = Binary_Search(Cur,key);
    Cur->key[i] = key;
    return ;
    }


    if(key < Cur->key[0])
        i = 0;
    else
    {
        i= Binary_Search(Cur,key)+1;
    }
    if(flag == 2)
    {
        Cur->key[0] = key;
        return;
    }
    

    /*Because of Left/right overwrting must be diffrent*/
   
   // printf("Cur->key[i] : %d\n",Cur->key[i+1]);
   // printf("Cur-key_num :%d\n",Cur->key_num);
    Cur->key[i] = key;
    //printf("Replace !!\n");

    //printf("Replace i %d\n",i);
   // printf("key :%d\n",key);
    

    
}
int Redistribute(BPlusTreeNode *Cur)
{
    BPlusTreeNode *father = Cur->father;
    int Root_up_key,Move_key,index;
    BPlusTreeNode *next,*prev,*child,*child2;
   // printf("Redistribute!!\n");
//    printf("father->key_num : %d\n",father->key_num);

  //  printf("cur key :%d\n",Cur->key[Cur->slot_array[1]]);
   // printf("father->key[0]: %d\n",father->key[0]);
   // printf("Cur->key[0] : %d\n",Cur->key[0]);

    if(father->key[0] > Cur->key[0])
    {
        index = 0;
    }
    else
    {
      index = Binary_Search(father,Cur->key[0])+1;
    }

           // printf("index : %d\n",index);

    if(Cur->isLeaf == false)
    {
       // printf("----------------non----leaf------\n");
        
        if(index == 0)
        {
           // printf("Non_Leaf_distribute_hi\n");
            next = father->child[index+1];
            prev = NULL;
            if(next->key_num <= 2)
            {
                return false;
            }
            else
            { 
                int i,ins;
                Move_key = Finding_Moving_Child(father,next);
               // printf("Move_key : %d\n",Move_key);
                child = Find_Child(next,Move_key);
                child->father = Cur;
                Merge_Node(Cur,Move_key,child);

                Root_up_key = next->key[0];

                replace(father,Root_up_key,5);

                ins = Binary_Search(next,Root_up_key);
                for(i=ins; i<next->key_num-1; i++)
                {
                    next->key[i] = next->key[i+1];
                    next->child[i] = next->child[i+1];
                }
                next->child[i] = next->child[i+1];
                next->key_num = next->key_num - 1;


                return true;
            }
        }
        else if(index == father->key_num)
        {
           // printf("hi1\n");

            // sleep(10);

            if(index == 0)
            {

                prev = father->child[0];
            }
            else
            {
                prev = father->child[index-1];//prev
            }
            next = NULL;

           // printf("father key : %d\n",father->key[0]);
           // printf("prev :%d\n",prev->key[0]);

            if(prev->key_num  <= 2)
                return false;
            else
            {

                Root_up_key = Finding_Moving_Child(father,Cur);
               // printf("Root_up_key: %d\n",Root_up_key);
                // child = Cur->child[0];
                // printf("child->key[0] : %dd\n",child->key[0]);

                // child = Find_Child(Cur,Root_up_key);
                // child->father = Cur;
                // Merge_Node(Cur,Root_up_key,child);


                Move_key = prev->key[prev->key_num-1];
               // printf("Move_key :%d\n",Move_key);
                child2 = Find_Child(prev,Move_key);
                child2->father = Cur;
                Merge_Node(Cur,Root_up_key,child2);



                Re_Delete(prev,Move_key);
                // Re_Delete(Cur,Move_key);


                replace(father,Move_key,3);





                return true;
            }
        }
        else
        {
           // printf("hi2\n");
            if(index == 0)
            {
                prev = father->child[0];
            }
            else
            {
                prev = father->child[index-1];
            }

            next = father->child[index+1];

           // printf("prev :%d\n",prev->key[0]);
           // printf("next :%d\n",next->key[0]);
            if(prev->key_num <= 2 && next->key_num <= 2)
                return false;
            else
            {
                if(prev->key_num <=2 && prev->key_num < next->key_num)
                {
                    int i,ins;
                   // printf("wow\n");
                    Move_key = Finding_Moving_Child(father,next);
                  //  printf("Move_key : %d\n",Move_key);
                    child = Find_Child(next,Move_key);
                    child->father = Cur;
                    Merge_Node(Cur,Move_key,child);
 
                    Root_up_key = next->key[0];
                   
                    replace(father,Root_up_key,5);

                    ins = Binary_Search(next,Root_up_key);
                    for(i=ins; i<next->key_num-1; i++)
                    {
                        next->key[i] = next->key[i+1];
                        next->child[i] = next->child[i+1];
                    }
                        next->child[i] = next->child[i+1];
                    next->key_num = next->key_num - 1;


                    // printf("next->key[next->slot_array[1] : %d\n",next->key[next->slot_array[1]]);
                    // printf("Move_key : %d\n",Move_key);
                    return true;
                }
                else
                {
                   // printf("Here\n");
                    //sleep(10);
                    //Insert(Cur,prev->key[prev->slot_array[prev->key_num]],prev->child[prev->slot_array[prev->key_num]]);
                    //Re_Delete(prev,prev->key[prev->slot_array[prev->key_num]]);
                    //replace(father,prev->key[prev->slot_array[prev->key_num]],4);
                    //Re_Delete(prev,prev->key[prev->slot_array[prev->key_num]]);

         
               // printf("Cur->key[0]---: %id\n",Cur->key[0]);

                     Root_up_key = Finding_Moving_Child(father,Cur);
                       // printf("Root_up_key: %d\n",Root_up_key);
                       // child = Cur->child[0];
                       // printf("child->key[0] : %dd\n",child->key[0]);

                   // child = Find_Child(Cur,Root_up_key);
                   // child->father = Cur;
                   // Merge_Node(Cur,Root_up_key,child);


                     Move_key = prev->key[prev->key_num-1];
                  // printf("Move_key :%d\n",Move_key);
                    child2 = Find_Child(prev,Move_key);
                    child2->father = Cur;
                    Merge_Node(Cur,Root_up_key,child2);
                  


                    Re_Delete(prev,Move_key);
                   // Re_Delete(Cur,Move_key);


                    replace(father,Move_key,3);



                    return true;
                }

            }
        }



    }

    else{
/************************/
        //printf("Normal redistribute\n");
    if(index == 0)
    {
        //printf("hi\n");

        next = father->child[index+1];
        prev = NULL;
        //printf("next->key %d\n",next->key[0]);
        //printf("next->key_num : %d\n",next->key_num);
        if(next->key_num <= 2)
        {
         //   printf("out\n");
            return false;
        }
        else
        {
          //  printf("next :%d\n",next->key[next->slot_array[1]]);

            Merge_Node(Cur,next->key[0],next->child[0]);
            Re_Delete(next,next->key[0]);
            replace(father,next->key[0],2);
            return true;
        }
    }
    else if(index == father->key_num)
    {
//        printf("hi1\n");


        if(index == 0)
        {
         //   printf("exi!\n");
            prev = father->child[0];
        }
        else
        {
           // printf("dxei!\n");
            prev = father->child[index-1];//prev
        }
       // printf("index : %d\n",index);

        next = NULL;

        //printf("father key : %d\n",father->key[father->slot_array[1]]);
       // printf("prev :%d\n",prev->key[prev->slot_array[1]]);

        if(prev->key_num  <= 2)
            return false;
        else
        {
         //   printf("hi error!!\n");

            
            Merge_Node(Cur,prev->key[prev->key_num-1],prev->child[prev->key_num-1]);
            replace(father,prev->key[prev->key_num-1],4);
            Re_Delete(prev,prev->key[prev->key_num-1]);
            // Insert(father,prev->key[prev->slot_array[prev->key_num]],Cur);


            return true;
        }
    }
    else
    {
       // printf("hi2\n");
       // printf("index: %d\n",index);
        if(index == 0)
        {
            prev = father->child[0];
        }
        else
        {
            prev = father->child[index-1];
        }

        next = father->child[index+1];
       // next = Cur->next;
       // printf("prev :%d\n",prev->key[0]);
       // printf("next :%d\n",next->key[0]);
        if(prev->key_num <= 2 && next->key_num <= 2)
            return false;
        else
        {
            if(prev->key_num == 2 && prev->key_num < next->key_num)
            {
               // printf("insight\n");
               // sleep(10);
                //Debug_Print(Cur);
                //Debug_Print(next);
                Merge_Node(Cur,next->key[0],next->child[0]);
            Re_Delete(next,next->key[0]);

            replace(father,next->key[0],5);
                //Debug_Print(next);

                
            }
            else
            {
                //printf("Here\n");
      //          printf("prev->key: %d\n",prev->key[prev->key_num-1]);
                Merge_Node(Cur,prev->key[prev->key_num-1],prev->child[prev->key_num-1]);
                //Re_Delete(prev,prev->key[prev->slot_array[prev->key_num]]);
                replace(father,prev->key[prev->key_num-1],4);
                Re_Delete(prev,prev->key[prev->key_num-1]);
                return true;
            }

        }
    }

    //printf("index : %d\n",index);

    //printf("cur key :%d\n",Cur->key[Cur->slot_array[1]]);
    //printf("father : %p\n",father);
    // printf("father key : %d\n",father->key[father->slot_array[1]]);
    // printf("next :%d\n",next->key[next->slot_array[1]]);
   // printf("prev :%d\n",prev->key[prev->slot_array[1]]);
   // printf("next->key_num: %d\n",next->key_num);
    //printf("prev->key_num: %d\n",prev->key_num);
   // sleep(5);
    
    }

}
BPlusTreeNode* Find(int key, int modify) {
    BPlusTreeNode* Cur = Root;
    while (1) {
        if (Cur->isLeaf == true)
            break;
        if (key < Cur->key[0]) {
            Cur = Cur->child[0];
        } else {
            int i = Binary_Search(Cur, key);
            // printf("what is i : %d\n",i);
            Cur = Cur->child[i+1];
        }
    }
    return Cur;
}

/** Destroy subtree whose root is Cur, By recursion */
void Destroy(BPlusTreeNode* Cur) {
	if (Cur->isLeaf == true) {
		int i;
		for (i = 0; i < Cur->key_num; i++)
			free(Cur->child[i]);
	} else {
		int i;
		for (i = 0; i < Cur->key_num; i++)
			Destroy(Cur->child[i]);
	}
	free(Cur);
}

/** Print subtree whose root is Cur */
void Print(BPlusTreeNode* Cur) {
	int i;
	for (i = 0; i < Cur->key_num; i++)
		printf("%d ", Cur->key[i]);
	printf("\n");
	if (!Cur->isLeaf) {
		for (i = 0; i < Cur->key_num; i++)
			Print(Cur->child[i]);
	}
}

/** Interface: Insert (key, value) into B+tree */
int BPlusTree_Insert(int key, int pos, void* value) {
	BPlusTreeNode* Leaf = Find(key, true);
	int i = Binary_Search(Leaf, key);
	if (Leaf->key[i] == key) return false;
	Insert(Leaf, key, pos, value);
	return true;
}

/** Interface: query all record whose key satisfy that key = query_key */
void BPlusTree_Query_Key(int key) {
	BPlusTreeNode* Leaf = Find(key, false);
	QueryAnsNum = 0;
	int i;
	for (i = 0; i < Leaf->key_num; i++) {
		//printf("%d ", Leaf->key[i]);
		if (Leaf->key[i] == key) {
			QueryAnsNum++;
			if (QueryAnsNum < 20) printf("[no.%d	key = %d, value = %s]\n", QueryAnsNum, Leaf->key[i], (char*)Leaf->child[i]);
		}
	}
	printf("Total number of answers is: %d\n", QueryAnsNum);
}

/** Interface: query all record whose key satisfy that query_l <= key <= query_r */
void BPlusTree_Query_Range(int l, int r) {
	BPlusTreeNode* Leaf = Find(l, false);
	QueryAnsNum = 0;
	int i;
	for (i = 0; i < Leaf->key_num; i++) {
		//printf("%d ", Leaf->key[i]);
		if (Leaf->key[i] >= l) break;
	}
	int finish = false;
	while (!finish) {
		while (i < Leaf->key_num) {
			if (Leaf->key[i] > r) {
				finish = true;
				break;
			}
			QueryAnsNum++;
			if (QueryAnsNum == 20) printf("...\n");
			if (QueryAnsNum < 20) printf("[no.%d	key = %d, value = %s]\n", QueryAnsNum, Leaf->key[i], (char*)Leaf->child[i]);
			i++;
		}
		if (finish || Leaf->next == NULL) break;
		Leaf = Leaf->next;
		i = 0;
	}
	printf("Total number of answers is: %d\n", QueryAnsNum);
}

/** Interface: Find the position of given key */
int BPlusTree_Find(int key) {
	BPlusTreeNode* Leaf = Find(key, false);
	int i = Binary_Search(Leaf, key);
	if (Leaf->key[i] != key) return -1; // don't have this key
	return 2;
}

/** Interface: modify value on the given key */
void BPlusTree_Modify(int key, void* value) {
	BPlusTreeNode* Leaf = Find(key, false);
	int i = Binary_Search(Leaf, key);
	if (Leaf->key[i] != key) return; // don't have this key
	printf("Modify: key = %d, original value = %s, new value = %s\n", key, (char*)(Leaf->child[i]), (char*)(value));
	free(Leaf->child[i]);
	Leaf->child[i] = value;
}

/** Interface: delete value on the given key */
int BPlusTree_Delete(int key) {
	BPlusTreeNode* Leaf = Find(key, false);
	int i = Binary_Search(Leaf, key);
	if (Leaf->key[i] == key){
            Delete(Leaf, key); 
            return 1;
        }
        else
        {
            return -2;
        }
}

/** Interface: Called to destroy the B+tree */
void BPlusTree_Destroy() {
	if (Root == NULL) return;
	printf("Now destroying B+tree ..\n");
	Destroy(Root);
	Root = NULL;
	printf("Done.\n");
}

/** Interface: Initialize */
void BPlusTree_Init() {
	BPlusTree_Destroy();
	Root = New_BPlusTreeNode();
	Root->isRoot = true;
	Root->isLeaf = true;
	TotalNodes = 0;
}

/**
 * Interface: setting MaxChildNumber in your program
 * A suggest value is cube root of the no. of records
 */
void BPlusTree_SetMaxChildNumber(int number) {
	MaxChildNumber = number + 1;
}

/** Interface: print the tree (DEBUG use)*/
void BPlusTree_Print() {
        int cnt = 0;
        BPlusTreeNode* root_first = Root;
        BPlusTreeNode* Leaf = NULL;
        BPlusTreeNode *before,*origin;
        printf("Print start!!\n");
	while (root_first->isLeaf == false) {

            int i;
            printf("-----Print key-----\n");
    
            for(i = 0; i<MAX_CHILD_NUMBER; i++)
                {
                printf("%4d", root_first->key[i]);
                if (++cnt % MAX_CHILD_NUMBER  == 0) printf("\n");
            }
                       cnt = 0;
            printf("\n");
            for(i=0; i<MAX_CHILD_NUMBER+1; i++)
            {
                printf("%p ",root_first->child[i]);
                if (++cnt % MAX_CHILD_NUMBER+1 == 0) printf("\n");
            }
            printf("\n");
            printf("root_first->key[0] :%d\n",root_first->key[0]);
            printf("root_first->key_num : %d\n",root_first->key_num);
            printf("root_first->child[0] :%p\n",root_first->child[0]);
            printf("Root address : %p\n",Root);
            printf("My_addrss : %p\n",root_first);
            printf("Next : %p\n",root_first->next);
            printf("father : %p\n",root_first->father);
            
            if(root_first->isRoot == true )
            {
                root_first = root_first->child[0];
                before = root_first;
            
            }
            else
            {
            root_first = root_first->next;
            if(root_first == NULL)
            {
                root_first = before->child[0];
                before = root_first;
            }
            
            }
           
            
            

             
            printf("*************************Non-leaf************************\n");
        }


        Leaf = Root;
        while(1)
        {
        if(Leaf->isLeaf == true)
            break;
        else
        {
            Leaf = Leaf->child[0];
        }
        }
        while (Leaf != NULL) {

            int i;
            printf("-----Print key-----\n");

            for(i = 0; i<MAX_CHILD_NUMBER; i++)
            {
                printf("%4d ", Leaf->key[i]);
                if (++cnt % MAX_CHILD_NUMBER+1 == 0) printf("\n");
            }
            printf("\n");
            
            cnt = 0;
            printf("\n");
            for(i=0; i<MAX_CHILD_NUMBER+1; i++)
            {
                printf("%s ",(char *)Leaf->child[i]);
                if (++cnt % MAX_CHILD_NUMBER+1 == 0) printf("\n");
            }
            printf("\n");
            printf("Leaf->key_num :%d\n",Leaf->key_num);
            printf("First key: %d\n",Leaf->key[0]);
           printf("Address of leaf : %p\n",Leaf);
           printf("Address of next : %p\n",Leaf->next);
           printf("Address of father: %p\n",Leaf->father);
            //printf("Leaf->child[0] : %s\n",Leaf->child[0]);
            printf("*******************leaf*****************\n");
            Leaf = Leaf->next;
        }
        printf("Total nodes : %d\n",TotalNodes);

}

/** Interface: Total Nodes */
int BPlusTree_GetTotalNodes() {
	return TotalNodes;
}

int BPlusTree_GetSplitCount() {
    return split_count;
}


