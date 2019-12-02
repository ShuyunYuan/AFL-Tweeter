#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>

struct entry_s {
    char *key;
    int value;
    struct entry_s *next;
};

typedef struct entry_s entry_t;

struct pair_s {
    char *key;
    int value;
};

typedef struct pair_s pair_t;

struct hashtable_s {
    int size;
    struct entry_s **table;
};

typedef struct hashtable_s hashtable_t;


/* Create a new hashtable. */
hashtable_t *ht_create(int size) {

    hashtable_t *hashtable = NULL;
    int i;

    if (size < 1) return NULL;

    /* Allocate the table itself. */
    if ((hashtable = malloc(sizeof(hashtable_t))) == NULL) {
        return NULL;
    }

    /* Allocate pointers to the head nodes. */
    if ((hashtable->table = malloc(sizeof(entry_t *) * size)) == NULL) {
        return NULL;
    }
    for (i = 0; i < size; i++) {
        hashtable->table[i] = NULL;
    }

    hashtable->size = size;

    return hashtable;
}

/* Hash a string for a particular hash table. */
int ht_hash(hashtable_t *hashtable, char *key) {

    unsigned long int hashval;
    int i = 0;

    /* Convert our string to an integer */
    while (hashval < ULONG_MAX && i < strlen(key)) {
        hashval = hashval << 8;
        hashval += key[i];
        i++;
    }

    return hashval % hashtable->size;
}

/* Create a key-value pair. */
entry_t *ht_newpair(char *key, int value) {
    entry_t *newpair;

    if ((newpair = malloc(sizeof(entry_t))) == NULL) {
        return NULL;
    }

    if ((newpair->key = strdup(key)) == NULL) {
        return NULL;
    }

    newpair->value = value;
    /*if ((newpair->value = value) == NULL) {
        return NULL;
    }*/

    newpair->next = NULL;

    return newpair;
}

/* Insert a key-value pair into a hash table. */
void ht_set(hashtable_t *hashtable, char *key, int value) {
    int bin = 0;
    entry_t *newpair = NULL;
    entry_t *next = NULL;
    entry_t *last = NULL;

    bin = ht_hash(hashtable, key);

    next = hashtable->table[bin];

    while (next != NULL && next->key != NULL && strcmp(key, next->key) > 0) {
        last = next;
        next = next->next;
    }

    /* There's already a pair.  Let's replace that string. */
    if (next != NULL && next->key != NULL && strcmp(key, next->key) == 0) {

        //free(next->value);
        next->value = value;

        /* Nope, could't find it.  Time to grow a pair. */
    } else {
        newpair = ht_newpair(key, value);

        /* We're at the start of the linked list in this bin. */
        if (next == hashtable->table[bin]) {
            newpair->next = next;
            hashtable->table[bin] = newpair;

            /* We're at the end of the linked list in this bin. */
        } else if (next == NULL) {
            last->next = newpair;

            /* We're in the middle of the list. */
        } else {
            newpair->next = next;
            last->next = newpair;
        }
    }
}

/* Retrieve a key-value pair from a hash table. */
int ht_get(hashtable_t *hashtable, char *key) {
    int bin = 0;
    entry_t *pair;

    bin = ht_hash(hashtable, key);

    /* Step through the bin, looking for our value. */
    pair = hashtable->table[bin];
    while (pair != NULL && pair->key != NULL && strcmp(key, pair->key) > 0) {
        pair = pair->next;
    }

    /* Did we actually find anything? */
    if (pair == NULL || pair->key == NULL || strcmp(key, pair->key) != 0) {
        return -1;

    } else {
        return pair->value;
    }
}

void display(hashtable_t *hashtable) {
    int bin = 0;
    entry_t *pair;

    for (bin = 0; bin < hashtable->size; bin++) {
        pair = hashtable->table[bin];
        while (pair != NULL && pair->key != NULL) {
            printf(" (%s, %d)", pair->key, pair->value);
            pair = pair->next;
        }
        // printf(" ~~ ");
    }

    printf("\n");
}

// get the top 10 tweeters, 10*O(n)
pair_t *sort(hashtable_t *hashtable, int top, int n) {

    pair_t topTweeter[top];

    if (top > n) {
        return sort(hashtable, n, n);
    }

    int bin = 0;
    entry_t *curpair;
    entry_t *maxpair = ht_newpair("dummy", 0);

    for (int i = 0; i < top; i++) {
        for (bin = 0; bin < hashtable->size; bin++) {
            curpair = hashtable->table[bin];
            while (curpair != NULL && curpair->key != NULL) {
                if (curpair->value > maxpair->value) {
                    maxpair = curpair;
                }
                curpair = curpair->next;
            }
        }

        // todo: add maxpair to ith topTweeter list
        topTweeter[i].key = maxpair->key;
        topTweeter[i].value = maxpair->value;

        // todo: remove the current max tweeter count from hashtable (set to 0)
        ht_set(hashtable, maxpair->key, 0);

    }

    // todo: re-add the top tweeter count back to hashtable
    for (int i = 0; i < top; i++) {
        ht_set(hashtable, topTweeter[i].key, topTweeter[i].value);
        printf("%s: %d\n", topTweeter[i].key, topTweeter[i].value);
    }

    return topTweeter;
}
/*
void print(pair_t *list) {
    for (int i = 0; i < (int)sizeof(list) / sizeof(pair_t); i++) {
        printf("%s: %d", list[i].key, list[i].value);
    }
}*/

// manually parse input csv, to not skip the empty column
char *zStrtok(char *str, const char *delim) {
    static char *static_str = 0;      /* var to store last address */
    int index = 0, strlength = 0;           /* integers for indexes */
    int found = 0;                  /* check if delim is found */

    /* delimiter cannot be NULL
    * if no more char left, return NULL as well
    */
    if (delim == 0 || (str == 0 && static_str == 0))
        return 0;

    if (str == 0)
        str = static_str;

    /* get length of string */
    while (str[strlength])
        strlength++;

    /* find the first occurrence of delim */
    for (index = 0; index < strlength; index++)
        if (str[index] == delim[0]) {
            found = 1;
            break;
        }

    /* if delim is not contained in str, return str */
    if (!found) {
        static_str = 0;
        return str;
    }

    /* check for consecutive delimiters
    *if first char is delim, return delim
    */
    if (str[0] == delim[0]) {
        static_str = (str + 1);
        return (char *) delim;
    }

    /* terminate the string
    * this assignment requires char[], so str has to
    * be char[] rather than *char
    */
    str[index] = '\0';

    /* save the rest of the string */
    if ((str + index + 1) != 0)
        static_str = (str + index + 1);
    else
        static_str = 0;

    return str;
}

int checkHeader(char *text) {//check quotes and remove possible quotes
    int quote = 0;
    //check if text contain char '\n' and remove it
    int len = strlen(text);
    if (text[len - 1] == '\n') {
        strncpy(text, &text[0], len - 1);
        text[len - 1] = '\0';
        len = len - 1;
    }
    //printf("%c %c\n", text[0], text[strlen(text) - 1]);
    if (text[0] == '\'' || text[0] == '"' || text[len - 1] == '\'' || text[len - 1] == '"') {

        if (text[len - 1] != text[0] || len < 2) {
            return -1;
        }
        if (text[0] == '\'') {
            quote = 1;
        } else {
            quote = 2;
        }
        strncpy(text, &text[1], len - 2);
        text[len - 2] = '\0';
        //printf("%s\n", text);
    } //now the text contain column header without quotes

    //to lowercase
    for (int i = 0; i < strlen(text); i++) {
        text[i] = tolower(text[i]);
    }
    return quote;
}

int checkQuote(char *text, int quote) {
    //printf("quote: %d\n", quote);
    if (text[strlen(text) - 1] == '\n') {
        int len = strlen(text);
        strncpy(text, &text[0], len - 1);
        text[len - 1] = '\0';
    }
    if (quote == 1) {
        // with ''
        if (text[0] != '\'' || text[strlen(text) - 1] != '\'') {
            return -1;
        } else {
            int len = strlen(text) - 2;
            strncpy(text, &text[1], len);
            text[len] = '\0';
        }
    } else if (quote == 2) {
        //with ""
        if (text[0] != '"' || text[strlen(text) - 1] != '"') {
            return -1;
        } else {
            int len = strlen(text) - 2;
            strncpy(text, &text[1], len);
            text[len] = '\0';
        }
    } //else: no quote or empty header
    return 1;
}

int main(int argc, char const *argv[]) {
    char str[20];
    //get input csv filename
    printf("Please input csv file name. \n");
    while (scanf("%s", str) != 1) {
        printf("Please input csv file name.\n");
        scanf("%s", str);
    }
    printf("File name: %s\n", str);

    //check whether input name is a ".csv" file
    char tail[5];
    strncpy(tail, &str[strlen(str) - 4], 4);
    tail[4] = '\0';
    //printf("%s\n", tail);
    if (strcmp(tail, ".csv") != 0) {
        printf("Invalid Input Format: Not a CSV file\n");
        exit(1);
    }

    //check whether this file exist in this dirctory
    FILE *fp = fopen(str, "r");
    if (!fp) {
        printf("Invalid Input Format: Can't open file\n");
        exit(1);
    }

    // the length of the input data file will not exceed 20,000 lines
    hashtable_t *hashtable = ht_create(20000);
    hashtable_t *headertable = ht_create(512); //<header, location>
    int column[512];

    char buf[1024];
    int header = 0;
    int row_count = 0;
    int field_count = 0;
    int name_col = 0;
    int tweeter_count = 0;
    while (fgets(buf, 1025, fp)) {
        //check length of each line, must less or equal to 1024
        //printf("%d\n", strlen(buf));
        if (strlen(buf) > 1024) {
            printf("Invalid Input Format: Too many characters in one line \n");
            exit(1);
        }
        if (strcmp(buf, "\n") == 0) {
            //printf("empty line, go to a new line\n");
            continue;
        }

        field_count = 0;
        row_count++;
        char *field = zStrtok(buf, ",");

        if (header == 0) {
            while (field) {
                //printf("%s\n", field);
                if (strcmp(field, ",") != 0) { //found a valid column header
                    header = 1;
                    char text[strlen(field) + 1];
                    strcpy(text, field);
                    text[strlen(field)] = '\0';
                    //printf("%s\n", text);
                    int quote = checkHeader(text); //0: no quote, 1: with quote '', 2: with quote ""
                    if (quote < 0) { //quotes matching error
                        printf("Invalid Input Format: quotes not match\n");
                        exit(1);
                    }
                    //printf("%s\n", text);
                    if (ht_get(headertable, text) >= 0) { //this header exists already -> invalid file
                        printf("Invalid Input Format: duplicate header\n");
                        exit(1);
                    }
                    ht_set(headertable, text, field_count);
                    column[field_count] = quote;
                    //printf("header: %s, quote: %d\n", text, column[field_count]);
                }
                field = zStrtok(NULL, ",");
                field_count++;
            }
            if (ht_get(headertable, "name") < 0) {
                printf("Invalid Input Format: cannot find \"name\" header\n");
                exit(1);
            }
            name_col = ht_get(headertable, "name");
        } else {
            while (field) {
                char text[strlen(field) + 1];
                //check whether the quotes match its header
                if (strcmp(field, ",") != 0) {
                    strcpy(text, field);
                    text[strlen(field)] = '\0';
                } else {
                    text[0] = '\0';
                }
                //printf("%s\n", text);
                if (checkQuote(text, column[field_count]) == -1) {
                    printf("Invalid Input Format: quotes doesn't match header \n");
                    exit(1);
                }
                //printf("%s\n", text);
                if (field_count == name_col) {
                    //printf("Tweeter Name:\t");
                    //printf("%s\n", text);
                    int value = 0;
                    if (ht_get(hashtable, text) >= 0) //already met this name
                        value = ht_get(hashtable, text);
                    else
                        tweeter_count++;
                    ht_set(hashtable, text, value + 1);
                    
                    //printf("%d\n", value);
                    //printf("%d\n", ht_get(hashtable, text));
                }
                field = zStrtok(NULL, ",");
                field_count++;
            }
        }
    }
    //display(hashtable);

    fclose(fp);

    pair_t *topTweeter = sort(hashtable, 10, tweeter_count);
    // print(topTweeter);

    return 0;
}