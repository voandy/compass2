/* 

Programmer: Andy Vo, voa1@student.unimelb.edu.au

--GLOSSARY--

Dictionary: An array of entries. I.e. dictionary[]

Entry: An single element in the dictionary. Represented by the dictionary_t 
struct. Contains a word and its associated probabilities.

Name: A special type of word. Whether a word qualifies to be a name is 
algorithmically by its context in sentence and the probabilities in dictionary.

Sentence: A linked-list of single-word strings.

Word: Used flexibly but always refers to a single-word string. May refer to an 
element of the sentence or the word_ref of an entry.

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_WORD_LENGTH 30 /* max chars in a word */
#define MAX_DICT_SIZE 100 /* max entries in dictionary */
#define MAX_LABEL_LENGTH 20 /* max chars in name labels */

typedef char word_t[MAX_WORD_LENGTH];
#include "listops.c"

/* stage numbers */
#define STAGE_NUM_ONE 1 
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_NUM_FIVE 5

/* string constants */
#define DIV "=========================" /* stage header */
#define FIRST "FIRST_NAME"
#define LAST "LAST_NAME"
#define NOT "NOT_NAME"

/* struct to store properties of an entry in the dict */
typedef struct dictionary_t{
	char word_ref[MAX_WORD_LENGTH];
	int p_first;
	int p_last;
	int p_non_name;
} dictionary_t;

/* function prototypes */
int read_dictionary(dictionary_t dictionary[]);
void read_sentence(list_t *sentence);
void print_stage_header(int stage_no);
void stage_one (dictionary_t dictionary[]);
void print_entry (dictionary_t dictionary[], int word_no);
void stage_two(dictionary_t dictionary[], int dict_size);
void stage_three(list_t *sentence);
void stage_four(dictionary_t dictionary[], int dict_size, list_t *sentence);
int word_dictionary_comp(const void *w, const void *d);
void print_naive_labels(word_t curr_word, dictionary_t* entry_address);
void stage_five(dictionary_t dictionary[], int dict_size, list_t *sentence);

/* main program */
int
main(int argc, char *argv[]) {
	dictionary_t dictionary[MAX_DICT_SIZE]; /* stores dictionary entries */
	list_t *sentence = make_empty_list(); /* stores words from sentence */
	int dict_size;

	/* reads our entries into dictionary*/
	dict_size = read_dictionary(dictionary);

	/* reads words into sentence */
	read_sentence(sentence);

	/* stage 1 */
	stage_one(dictionary);
	
	/* stage 2 */
	stage_two(dictionary, dict_size);

	/* stage 3 */
	stage_three(sentence);

	/* stage 4 */
	stage_four(dictionary, dict_size, sentence);

	/* stage 5 */
	stage_five(dictionary, dict_size, sentence);

	free_list(sentence);

	return 0;
}

/* reads in entries to populate dictionary and returns no. of entries*/
int 
read_dictionary(dictionary_t dictionary[]) {
	int dict_size = 0;
	word_t curr_word;
	char hash; /* stores a char, used to remove # and check for % in input */

	int i;
	for (i = 0; "Algorithms are fun!"; i++) {

		/* individually reads in suffix (#/%) before reading in name */
		scanf("%c%s", &hash, curr_word);

		/* breaks the loop when the end of the dict is reached */
		if (hash == '%') {
			break;
		}

		/* copies curr_word into our dictionary, copying after the break
		condition rather than in the scanf function prevents the % line from 
		being copied into our array */
		strcpy (dictionary[i].word_ref, curr_word);

		/* reads in probabilities */
		scanf("%d %d %d\n", &dictionary[i].p_first, &dictionary[i].p_last, 
			&dictionary[i].p_non_name);
	}
	dict_size = i;
	return dict_size;
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
stage_one (dictionary_t dictionary[]){
	print_stage_header(STAGE_NUM_ONE);
	print_entry(dictionary, 0);
}

/* prints a single entry from dictionary, seperated from stage 1 so it can be 
called for debugging */
void
print_entry (dictionary_t dictionary[], int entry_no) {
	printf("Word %d: %s\n", entry_no, dictionary[entry_no].word_ref);
	printf("Label probabilities: %d%% %d%% %d%%\n", 
		dictionary[entry_no].p_first, dictionary[entry_no].p_last, 
		dictionary[entry_no].p_non_name);
}

/* calculates and prints the average characters per word in dictionary */
void
stage_two(dictionary_t dictionary[], int dict_size) {
	print_stage_header(STAGE_NUM_TWO);

	int char_count = 0;
	double avg_count;

	/* counts total characters in all words and divides to get the mean */
	int i, j;
	for (i = 0; i < dict_size; i++) {
		for (j = 0; dictionary[i].word_ref[j] != '\0'; j++) {
			char_count++;
		}
	}
	avg_count = (float)char_count / (float)dict_size;

	printf("Number of words: %d\n", dict_size);
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

/* searches dictionary for each word in sentence
if the word is present we print FIRST_NAME and/or LAST_NAME if it has non-zero 
values for p_first/p_last */
void
stage_four(dictionary_t dictionary[], int dict_size, list_t *sentence) {
	print_stage_header(STAGE_NUM_FOUR);

	word_t curr_word; /* stores the word we are looking for */
	dictionary_t* entry_address; /* pointer to a match in dictionary if any */

	node_t* curr_node = sentence->head;
	while(curr_node) {
		strcpy(curr_word, curr_node->data);

		/* use a binary search to find curr_word in dictionary */
		entry_address = bsearch(curr_word, dictionary, dict_size, 
			sizeof(dictionary_t), word_dictionary_comp);

		/* prints the names with labels  */
		print_naive_labels(curr_word, entry_address);

		curr_node = curr_node->next;
	}
}

/* comparison function used for bsearch of dictionary, utilises strcmp to 
compare string at *w to string at *d->word_ref */
int 
word_dictionary_comp(const void *w, const void *d) {
   	return strncmp(w, ((const dictionary_t *)d)->word_ref, MAX_WORD_LENGTH);
}

/* prints each word in sentence with corresponding name labels */
void 
print_naive_labels(word_t curr_word, dictionary_t* entry_address) {
	/* this conditional takes advantage of the fact that bsearch returns 
	null if no match is found */
	if (entry_address) {
		printf("%-32s", curr_word);
		if (entry_address->p_first && entry_address->p_last) {
			printf("%s, %s\n", FIRST, LAST);
		}
		else if (entry_address->p_first) {
			printf("%s\n", FIRST);
		}
		else if (entry_address->p_last) {
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
stage_five(dictionary_t dictionary[], int dict_size, list_t *sentence) {
	print_stage_header(STAGE_NUM_FIVE);

	typedef struct namecalc_t{
		char word[MAX_WORD_LENGTH];
		int nametype;
	} namecalc_t;

	namecalc_t curr_word;
	namecalc_t prev_word;
	namecalc_t next_word;

	dictionary_t* entry_address;

	node_t* curr_node = sentence->head;
	while(curr_node) {
		curr_node = curr_node->next;
	}
}