#include "hashset.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[]){
    int echo = 0;                                // controls echoing, 0: echo off, 1: echo on
    if(argc > 1 && strcmp("-echo",argv[1])==0) { // turn echoing on via -echo command line option
        echo=1;
    }
    printf("Hashset Application\n");
    printf("Commands:\n");
    printf("  hashcode <elem>  : prints out the numeric hash code for the given key (does not change the hash set)\n");
    printf("  contains <elem>  : prints the value associated with the given element or NOT PRESENT\n");
    printf("  add <elem>       : inserts the given element into the hash set, reports existing element\n");
    printf("  print            : prints all elements in the hash set in the order they were addded\n");
    printf("  structure        : prints detailed structure of the hash set\n");
    printf("  clear            : reinitializes hash set to be empty with default size\n");
    printf("  save <file>      : writes the contents of the hash set to the given file\n");
    printf("  load <file>      : clears the current hash set and loads the one in the given file\n");
    printf("  next_prime <int> : if <int> is prime, prints it, otherwise finds the next prime and prints it\n");
    printf("  expand           : expands memory size of hash set to reduce its load factor\n");
    printf("  quit             : exit the program\n");

    char cmd[128];
    int prime;
    hashset_t hs;
    int success;
    hashset_init(&hs, 5);

    while(1){
        printf("HS>> ");
        success = fscanf(stdin, "%s", cmd);
        if(success == EOF){
            printf("\n");
            break;
        }
        if(strcmp("hashcode", cmd) == 0){
            fscanf(stdin, "%s", cmd);
            if(echo){
                printf("hashcode %s\n", cmd);
            }
            printf("%d\n", hashcode(cmd));
        }

        else if(strcmp("contains",cmd)==0){
            fscanf(stdin, "%s", cmd);
            if(echo){
                printf("contains %s\n", cmd);
            }
            success = hashset_contains(&hs, cmd);
            if(success){
                printf("FOUND: %s\n", cmd);
            }
            else{
                printf("NOT PRESENT\n");
            }
        }

        else if(strcmp("add", cmd) ==0){ //check for add command
            fscanf(stdin, "%s", cmd); //read string to add
            if(echo){
                printf("add %s\n", cmd);
            }
            success = hashset_add(&hs, cmd);
            if(!success){
                printf("Elem already present, no changes made\n");
            }
        }

        else if(strcmp("print",cmd)==0){
            if(echo){
                printf("print\n");
            }
            hashset_write_elems_ordered(&hs, stdout);
        }

        else if(strcmp("structure", cmd)==0){
            if(echo){
                printf("structure\n");
            }
            hashset_show_structure(&hs);
        }

        else if(strcmp("clear", cmd)==0){
            if(echo){
                printf("clear\n");
            }
            hashset_free_fields(&hs);
            hashset_init(&hs, 5);
        }

        else if(strcmp("save", cmd) == 0){
            fscanf(stdin, "%s", cmd);
            if(echo){
                printf("save %s\n", cmd);
            }
            hashset_save(&hs, cmd);
        }

        else if(strcmp("load", cmd) == 0){
            fscanf(stdin, "%s", cmd);
            if(echo){
                printf("load %s\n", cmd);
            }
            success = hashset_load(&hs,cmd);
            if(success == 0){
                printf("load failed\n");
            }
        }

        else if(strcmp("next_prime", cmd)==0){
            fscanf(stdin, "%d", &prime);
            if(echo){
                printf("next_prime %d\n", prime);
            }
            int ans = next_prime(prime);
            printf("%d\n", ans);
        }

        else if(strcmp("expand", cmd) == 0){
            if(echo){
                printf("expand\n");
            }
            hashset_expand(&hs);
        }


        else if(strcmp("quit", cmd)==0){ //check for quit command
            if(echo){
                printf("quit\n");
            }
            break; //break from loop
        }

        else{
            if(echo){
                printf("%s\n", cmd);
            }
            printf("unknown command %s\n", cmd);
        }
    }

    hashset_free_fields(&hs);
    return 0;
}
