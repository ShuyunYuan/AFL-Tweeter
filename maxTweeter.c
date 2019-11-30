#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

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
        return 0;
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

// todo: get the top 10 tweeters, 10*O(n)
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

void print(pair_t *list) {
    for (int i = 0; i < (int)sizeof(list) / sizeof(pair_t); i++) {
        printf("%s: %d", list[i].key, list[i].value);
    }
}

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

int main(int argc, char const *argv[]) {
    if (argc < 2) {
        printf("Please input csv file name.");
        return 1;
    }
    //if (argv[1])

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        printf("Invalid Input Format: Can't open file\n");
        return 1;
    }

    // the length of the input data file will not exceed 20,000 lines
    hashtable_t *hashtable = ht_create(20000);

    char buf[1024];
    int row_count = 0;
    int field_count = 0;
    int name_col = 0;
    int tweeter_count = 0;
    while (fgets(buf, 1024, fp)) {
        field_count = 0;
        row_count++;
        char *field = zStrtok(buf, ",");

        if (row_count == 1) {
            while (field) {
                if (strcmp(field, "name") == 0) {
                    name_col = field_count;
                    printf("%d\n", name_col);
                    break;
                }
                field = zStrtok(NULL, ",");
                field_count++;
            }
        } else {
            while (field) {
                if (field_count == name_col) {
                    printf("Tweeter Name:\t");
                    printf("%s\n", field);
                    int value = 0;
                    if (ht_get(hashtable, field) != 0)
                        value = ht_get(hashtable, field);
                    ht_set(hashtable, field, value + 1);
                    printf("%d\n", ht_get(hashtable, field));
                    tweeter_count++;
                    break;
                }

                field = zStrtok(NULL, ",");
                field_count++;
            }
            //if (row_count > 10)
            //	return 1;
        }
    }
    // display(hashtable);
    fclose(fp);

    pair_t *topTweeter = sort(hashtable, 10, tweeter_count);
    // print(topTweeter);

    return 0;
}