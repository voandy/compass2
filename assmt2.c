/* Programmer: Andy Vo, voa1@student.unimelb.edu.au

   Algorithms are fun!
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_NAME_LENGTH 30 /* max chars in a name */
#define MAX_DICT_LENGTH 100 /* max entries in dictionary */
#define MAX_LABEL_LENGTH 20 /* max chars in name labels */

typedef char word_t[MAX_NAME_LENGTH];
#include "listops.c"

#define DIV "=========================" /* stage header */

/* stage numbers */
#define STAGE_NUM_ONE 1 
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_NUM_FIVE 5

/* string constants */
const char FIRST[MAX_LABEL_LENGTH] = "FIRST_NAME";
const char LAST[MAX_LABEL_LENGTH] = "LAST_NAME";
const char NOT[MAX_LABEL_LENGTH] = "NOT_NAME";

/* struct to store properties of an entry in the dict */
typedef struct namedict_t{
	char name_ref[MAX_NAME_LENGTH];
	int p_first;
	int p_last;
	int p_non_name;
} namedict_t;

/* function prototypes */
int read_namedict(namedict_t namedict[]);
void read_sentence(list_t *sentence);
void print_stage_header(int stage_no);
void stage_one (namedict_t namedict[]);
void print_entry (namedict_t namedict[], int word_no);
void stage_two(namedict_t namedict[], int dict_length);
void stage_three(list_t *sentence);
void stage_four(namedict_t namedict[], int dict_length, list_t *sentence);
int word_namedict_comp(const void *w, const void *d);
void print_naive_labels(word_t curr_word, namedict_t* name_address);
void stage_five(namedict_t namedict[], int dict_length, list_t *sentence);

/* main program */
int
main(int argc, char *argv[]) {
	namedict_t namedict[MAX_DICT_LENGTH]; /* stores dictionary entries */
	list_t *sentence = make_empty_list(); /* stores words from sentence */
	int dict_length;

	/* reads our dictionary entries into namedict*/
	dict_length = read_namedict(namedict);

	/* reads words into sentence */
	read_sentence(sentence);

	/* stage 1 */
	stage_one(namedict);
	
	/* stage 2 */
	stage_two(namedict, dict_length);

	/* stage 3 */
	stage_three(sentence);

	/* stage 4 */
	stage_four(namedict, dict_length, sentence);

	/* stage 5 */
	stage_five(namedict, dict_length, sentence);

	return 0;
}

/* reads in entries to populate namedict and returns no. of entries*/
int 
read_namedict(namedict_t namedict[]) {
	int dict_length = 0;
	word_t curr_word;
	char hash; /* stores a char, used to remove # and check for % in input */

	int i;
	for (i = 0;; i++) {

		/* individually reads in suffix (#/%) before reading in name */
		scanf("%c%s", &hash, curr_word);

		/* breaks the loop when the end of the dict is reached */
		if (hash == '%') {
			break;
		}

		/* copies curr_word into our dictionary, copying after the break
		condition rather than in the scanf function prevents the % line from 
		being copied into our array */
		strcpy (namedict[i].name_ref, curr_word);

		/* reads in probabilities */
		scanf("%d %d %d\n", &namedict[i].p_first, &namedict[i].p_last, 
			&namedict[i].p_non_name);
	}
	dict_length = i;
	return dict_length;
}

/* reads words into our sentence linked list */
void
read_sentence(list_t *sentence) {
	word_t curr_word;
	while (scanf("%s", curr_word) != EOF) {
		insert_at_foot(sentence, curr_word);
	}
}

/* prints a stage header */
void
print_stage_header(int stage_no) {
	if (stage_no > STAGE_NUM_ONE) {
		printf("\n");
	}
	printf("%sStage %d%s\n", DIV, stage_no, DIV);
}

/* prints the first entry from our dictionary */
void
stage_one (namedict_t namedict[]){
	print_stage_header(STAGE_NUM_ONE);
	print_entry(namedict, 0);
}

/* prints a single entry from namedict, seperated from stage 1 so it can be 
called for debugging */
void
print_entry (namedict_t namedict[], int word_no) {
	printf("Word %d: %s\n", word_no, namedict[word_no].name_ref);
	printf("Label probabilities: %d%% %d%% %d%%\n", namedict[word_no].p_first, 
		namedict[word_no].p_last, namedict[word_no].p_non_name);
}

/* calculates and prints the average characters per word in namedict */
void
stage_two(namedict_t namedict[], int dict_length) {
	print_stage_header(STAGE_NUM_TWO);

	int char_count = 0;
	double avg_count;

	/* counts total characters in all names and divides to get the mean */
	int i, j;
	for (i = 0; i < dict_length; i++) {
		for (j = 0; namedict[i].name_ref[j] != '\0'; j++) {
			char_count++;
		}
	}
	avg_count = (float)char_count / (float)dict_length;

	printf("Number of words: %d\n", dict_length);
	printf("Average number of characters per word: %.2f\n", avg_count);
}

void
stage_three(list_t *sentence) {
	print_stage_header(STAGE_NUM_THREE);

	/* prints each word in the linked list given */
	word_t curr_word; 
	node_t* curr_node = sentence->head;
	while(curr_node) {
		strcpy(curr_word, curr_node->data);
		printf("%s\n", curr_word);
		curr_node = curr_node->next;
	}
}

/* searches namedict for each word in sentence, if the word is present we print
FIRST_NAME/LAST_NAME if it has non-zero values for p_first and/or p_last */
void
stage_four(namedict_t namedict[], int dict_length, list_t *sentence) {
	print_stage_header(STAGE_NUM_FOUR);

	word_t curr_word; /* stores the word we are looking for */
	namedict_t* name_address; /* pointer to match in dict if any */

	node_t* curr_node = sentence->head;
	while(curr_node) {
		strcpy(curr_word, curr_node->data);

		/* use a binary search to find curr_word in namedict */
		name_address = bsearch(curr_word, namedict, dict_length, 
			sizeof (namedict_t), word_namedict_comp);

		/* prints the names with labels  */
		print_naive_labels(curr_word, name_address);

		curr_node = curr_node->next;
	}
}

/* comparison function used for bsearch of namedict, utilises strcmp to 
compare string at *w to string at *d->name_ref */
int 
word_namedict_comp(const void *w, const void *d) {
   	return strncmp(w, ((const namedict_t *)d)->name_ref, MAX_NAME_LENGTH);
}

/* prints each word in sentence with corresponding name labels */
void 
print_naive_labels(word_t curr_word, namedict_t* name_address) {
	/* this conditional takes advantage of the fact that bsearch returns 
	null if no match is found */
	if (name_address) {
		printf("%-32s", curr_word);
		if (name_address->p_first && name_address->p_last) {
			printf("%s, %s\n", FIRST, LAST);
		}
		else if (name_address->p_first) {
			printf("%s\n", FIRST);
		}
		else if (name_address->p_last) {
			printf("%s\n", LAST);
		}
		else {
			printf("%s\n", NOT);
		}
	}
	/* if not match is found we can simply print NOT_NAME */
	else {
		printf("%-32s", curr_word);
		printf("%s\n", NOT);
	}
}

void
stage_five(namedict_t namedict[], int dict_length, list_t *sentence) {
	print_stage_header(STAGE_NUM_FIVE);
}