//hashset_funcs.c: support functions for the hashset_main program

#include"hashset.h"
#include <stdlib.h>
#include <string.h>



// }
    // PROVIDED: Compute a simple hash code for the given character
// string. The code is "computed" by casting the first 8 characters of
// the string to a numbers and returning their xor. The empty string
// has hash code 0. Longer strings will have numbers which are the
// integer interpretation of up to their first 8 bytes.  ADVANTAGE:
// constant time to compute the hash code. DISADVANTAGE: poor
// distribution for long strings; all strings with same first 8 chars
// hash to the same location.
int hashcode(char key[]){
  int hc = 0;
  for(int i=0; key[i]!='\0'; i++){
    hc = hc*31 + key[i];
  }
  return hc;
}


// Initialize the hash set 'hs' to have given size and elem_count
// 0. Ensures that the 'table' field is initialized to an array of
// size 'table_size' and is filled with NULLs. Also ensures that the
// first/last pointers are initialized to NULL
void hashset_init(hashset_t *hs, int table_size){
    hs->elem_count = 0;
    hs->table_size = table_size;
    hs->table = malloc(sizeof(hashnode_t)*table_size); //hs->table will hold pointers, which are 8 bits each
    for(int i = 0; i<table_size; i++){ //filling in hs->table with NULL values
        hs->table[i] = NULL;
    }
    hs->order_first = NULL;
    hs->order_last = NULL;
    return;
}


// Returns 1 if the parameter `elem` is in the hash set and 0
// otherwise. Uses hashcode() and field `table_size` to determine
// which index in table to search.  Iterates through the list at that
// table index using strcmp() to check for `elem`. NOTE: The
// `hashcode()` function may return positive or negative
// values. Negative values are negated to make them positive. The
// "bucket" (index in hs->table) for `elem` is determined by with
// 'hashcode(key) modulo table_size'.
int hashset_contains(hashset_t *hs, char elem[]){
    int index = ((abs(hashcode(elem))) % (hs->table_size));
    hashnode_t *ptr = hs->table[index];
    while (ptr != NULL){
        if(strcmp((ptr->elem), elem) == 0){
            return 1;
        }
        else{
            ptr = ptr->table_next;
        }
    }
    return 0;
}


// If the element is already present in the hash set, makes no changes
// to the hash set and returns 0. hashset_contains() may be used for
// this. Otherwise determines the bucket to add `elem` at via the same
// process as in hashset_contains() and adds it to the FRONT of the
// list at that table index. Adjusts the `hs->order_last` pointer to
// append the new element to the ordered list of elems. If this is the
// first element added, also adjsuts the `hs->first` pointer. Updates the
// `elem_count` field and returns 1 to indicate a successful addition.
//
// NOTE: Adding elems at the front of each bucket list allows much
// simplified logic that does not need any looping/iteration.
int hashset_add(hashset_t *hs, char elem[]){
    if(hashset_contains(hs, elem) == 1){
        return 0;
    }
    else{ //element is not already in the hashset
        hs->elem_count +=1;
        int index = abs(hashcode(elem) % hs->table_size);

        //create a new node
        hashnode_t *newNode = malloc(sizeof(hashnode_t));
        strcpy(newNode->elem, elem);
        newNode->order_next = NULL;

        //put the new node at the end of the order linked list (ORDER)
        hs->order_last = newNode;
        hashnode_t *ptr = hs->order_first;
        if(ptr == NULL){
            hs->order_first = newNode;
        }
        else{
            while(ptr->order_next != NULL){
                ptr = ptr->order_next;
            }
        ptr->order_next = newNode;
        }

        //place the new node (TABLE)
        if(hs->table[index] != NULL){
            newNode->table_next = hs->table[index];
            hs->table[index] = newNode;
        }
        else{
            newNode->table_next = NULL;
            hs->table[index] = newNode;
        }

    }
    return 1;
}


// De-allocates nodes/table for `hs`. Iterates through the ordered
// list of the hash set starting at the `order_first` field and
// de-allocates all nodes in the list. Also free's the `table`
// field. Sets all relevant fields to 0 or NULL as appropriate to
// indicate that the hash set has no more usable space. Does NOT
// attempt to de-allocate the `hs` itself as it may not be
// heap-allocated (e.g. in the stack or a global).
void hashset_free_fields(hashset_t *hs){
    hashnode_t *ptr = hs->order_first;
    hashnode_t *temp;
    while(ptr != NULL){
        temp = ptr->order_next;
        free(ptr);
        ptr = temp;
    }
    free(hs->table);
    hs->elem_count = 0;
    hs->table_size = 0;
    hs->table = NULL;
    hs->order_first = NULL;
    hs->order_last = NULL;
    return;
}


// Displays detailed structure of the hash set. Shows stats for the
// hash set as below including the load factor (element count divided
// by table_size) to 4 digits of accuracy.  Then shows each table
// array index ("bucket") on its own line with the linked list of
// elems in the bucket on the same line. 
void hashset_show_structure(hashset_t *hs){
    printf("elem_count: %d\n", hs->elem_count);
    printf("table_size: %d\n", hs->table_size);
    if(hs->order_first == NULL){
        printf("order_first: NULL\n");
    }
    else{
        printf("order_first: %s\n", hs->order_first->elem);

    }
    if(hs->order_last == NULL){
        printf("order_last : NULL\n");
    }
    else{
        printf("order_last : %s\n", hs->order_last->elem);
    }
    double load_factor = (float)(hs->elem_count)/(hs->table_size);
    printf("load_factor: %.4lf\n", load_factor);

    hashnode_t *ptr;
    for(int i = 0; i<hs->table_size; i++){
        ptr = hs->table[i];
        printf("[%2d] :",i);
        while(ptr != NULL){
            printf(" {%d %s ", hashcode(ptr->elem), ptr->elem);
            if(ptr->order_next != NULL){
                printf(">>%s}", ptr->order_next->elem);
            }
            else{
                printf(">>NULL}");
            }
            ptr = ptr->table_next;
        }
        printf("\n");
    }
    return;
}

// Outputs all elements of the hash set according to the order they
// were added. Starts at the `order_first` field and iterates through
// the list defined there. Each element is printed on its own line
// preceded by its add position with 1 for the first elem, 2 for the
// second, etc. Prints output to `FILE *out` which should be an open
// handle. NOTE: the output can be printed to the terminal screen by
// passing in the `stdout` file handle for `out`.
void hashset_write_elems_ordered(hashset_t *hs, FILE *out){
    hashnode_t *ptr = hs->order_first;
    int counter =1;
    while(ptr!=NULL){
        fprintf(out, "%3d %s\n",counter, ptr->elem);
        counter +=1;
        ptr = ptr->order_next;
    }
    return;
}


// Writes the given hash set to the given `filename` so that it can be
// loaded later.  Opens the file and writes its 'table_size' and
// 'elem_count' to the file. Then uses the hashset_write_elems_ordered()
// function to output all elems in the hash set into the file.
void hashset_save(hashset_t *hs, char *filename){
    FILE *fout = fopen(filename,"w");
    if(fout == NULL){
        printf("ERROR: could not open file '%s'\n", filename);
        return;
    }
    fprintf(fout, "%d %d\n", hs->table_size, hs->elem_count);
    hashset_write_elems_ordered(hs, fout);
    fclose(fout);
    return;
}


// Loads a hash set file created with hashset_save(). If the file
// cannot be opened, prints the message
// ERROR: could not open file 'somefile.hs'
//
// and returns 0 without changing anything. Otherwise clears out the
// current hash set `hs`, initializes a new one based on the size
// present in the file, and adds all elems from the file into the new
// hash set. Ignores the indices at the start of each line and uses
// hashset_add() to insert elems in the order they appear in the
// file. Returns 1 on successful loading (FIXED: previously indicated
// a different return value on success) . This function does no error
// checking of the contents of the file so if they are corrupted, it
// may cause an application to crash or loop infinitely.
int hashset_load(hashset_t *hs, char *filename){
    FILE *fin = fopen(filename, "r");
    if(fin == NULL){
        printf("ERROR: could not open file '%s'\n", filename);
        return 0;
    }
    int table_size=0;
    fscanf(fin, "%d %*d\n", &table_size);
    hashset_free_fields(hs);
    hashset_init(hs, table_size);
    char elem[64];
    while(1){
        int ret = fscanf(fin, "  %*d %s\n",elem);
        if(ret == EOF){
            break;
        }
        hashset_add(hs, elem);
    }
    fclose(fin);
    return 1;
}


// If 'num' is a prime number, returns 'num'. Otherwise, returns the
// first prime that is larger than 'num'. Uses a simple algorithm to
// calculate primeness: check if any number between 2 and (num/2)
// divide num. If none do, it is prime. If not, tries next odd number
// above num. Loops this approach until a prime number is located and
// returns this. Used to ensure that hash table_size stays prime which
// theoretically distributes elements better among the array indices
// of the table.
int next_prime(int num){
    for(int i = 2; i<num/2; i++){
        if ((num%i)==0){
            if((num+1)%2 !=0){
                return next_prime(num+1);
            }
            else{
                return next_prime(num+2);
            }
        }
    }
    return num;
}


// Allocates a new, larger area of memory for the `table` field and
// re-adds all current elems to it. The size of the new table is
// next_prime(2*table_size+1) which keeps the size prime.  After
// allocating the new table, all table entries are initialized to NULL
// then the old table is iterated through and all elems are added to
// the new table according to their hash code. The memory for the old
// table is de-allocated and the new table assigned to the hash set
// fields "table" and "table_size".  This function increases
// "table_size" while keeping "elem_count" the same thereby reducing
// the load of the hash table. Ensures that the memory associated with
// the old table is free()'d. Makes NO special effort to preserve old
// nodes: re-adds everything into the new table and then frees the old
// one along with its nodes. Uses functions such as hashset_init(),
// hashset_add(), hashset_free_fields() to accomplish the transfer.

void hashset_expand(hashset_t *hs){
    int newTableSize = next_prime(2*(hs->table_size)+1);
    hashset_t new;
    hashset_init(&new, newTableSize);
    hashnode_t *ptr = hs->order_first;
    while(ptr != NULL){
        hashset_add(&new, ptr->elem);
        ptr = ptr->order_next;
    }
    hashset_free_fields(hs);
    *hs = new;
}