/* 

Programmer: Andy Vo, voa1@student.unimelb.edu.au

--GLOSSARY--

Dictionary: An array of entries. I.e. dictionary[]

Entry: An single element in the dictionary. Represented by the dictionary_t 
struct. Contains a word and its associated probabilities.

Name: A special type of word. Whether a word qualifies to be a name is 
algorithmically by its context in sentence and the probabilities in dictionary.

Sentence: A linked-list of single-word strings.

Type: The classification of a word in sentence. Either "First Name," 
"Last Name" or "Non Name."

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
	word_t word_ref;
	int p_first;
	int p_last;
	int p_non_name;
} dictionary_t;

/* see stage 5. stores a word from sentence, its dictionary address if any 
and its nametype if any */
typedef struct word_details_t{
	word_t word;
	dictionary_t* entry_address; /* pointer to entry in dictionary */
	int nametype; /* 0 for non name, 1 for first name, 2 for last name */
} word_details_t;

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
word_details_t get_details(word_t curr_word, dictionary_t dictionary[], 
	int dict_size);
int type_one_word(word_details_t curr_details);
int type_first_word(word_details_t curr_details, word_details_t next_details);
int type_curr_word(word_details_t prev_details, word_details_t curr_details, 
	word_details_t next_details);
int type_last_word(word_details_t prev_details, word_details_t curr_details);
int type_highest_prob(int first, int last, int non);
void print_smart_label(word_details_t curr_word);

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
			/* possibly not necessary as there shouldn't be an entry in the dict
			with a 100% associated with non_name */
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

	word_t curr_word; /* the current word we are focused on */
	word_t next_word; /* the next word in the sentence */

	/* stores details of the previous, current and next word in sentence, see
	the typedef of word_details_t for details */
	word_details_t prev_details;
	word_details_t curr_details;
	word_details_t next_details;

	node_t* curr_node = sentence->head;

	/* details of first word added to curr_details */
	strcpy(curr_word, curr_node->data);
	curr_details = get_details(curr_word, dictionary, dict_size);

	/* special case for sentences with only one word */
	if (!curr_node->next) {
		curr_details.nametype = type_one_word(curr_details);
		print_smart_label(curr_details);
		return; /* exits the fucntion */
	}

	/* details of second word added to next_details */
	curr_node = curr_node->next;
	strcpy(curr_word, curr_node->data);
	next_details = get_details(curr_word, dictionary, dict_size);

	/* calculates the type of the first word */
	curr_details.nametype = type_first_word(curr_details, next_details);
	print_smart_label(curr_details);

	while (curr_node->next){
		strcpy(next_word, curr_node->next->data);

		/* cycles word details for next word */
		prev_details = curr_details;
		curr_details = next_details;
		next_details = get_details(next_word, dictionary, dict_size);

		/* calculates and prints the type of the current word */
		curr_details.nametype = type_curr_word(prev_details, curr_details, 
			next_details);
		print_smart_label(curr_details);

		curr_node = curr_node->next;
	}

	/* cycles details */
	prev_details = curr_details;
	curr_details = next_details;

	/* calculates the type of the last word */
	curr_details.nametype = type_last_word(prev_details, curr_details);
	print_smart_label(curr_details);

}

/* given and word and the dictionary returns a struct containing the word and
a pointer to its entry in the dictionary if any */
word_details_t 
get_details(word_t curr_word, dictionary_t dictionary[], 
	int dict_size) {
	word_details_t curr_details;

	strcpy(curr_details.word, curr_word);

	curr_details.entry_address = bsearch(curr_word, dictionary, dict_size, 
			sizeof(dictionary_t), word_dictionary_comp);

	/* initialises nametype to null, it will be calculated later */
	curr_details.nametype = 0;

	return curr_details;
}

/* for one word sentences the type of the word is simply the type with the 
highest probability */
int 
type_one_word(word_details_t curr_details) {
	int nametype = 0; /* stores name type */

	/* if the word isn't in the dictions simply return 0 */
	if (!curr_details.entry_address) {
		return nametype;
	}

	nametype = type_highest_prob(curr_details.entry_address->p_first, 
		curr_details.entry_address->p_last, 
		curr_details.entry_address->p_non_name);

	return nametype;
}

/* given the details of the first word and the word following this will
calculate and return the type of the first word */
int 
type_first_word(word_details_t curr_details, word_details_t next_details) {
	int nametype = 0;

	if (!curr_details.entry_address) {
		return nametype;
	}

	int curr_first = curr_details.entry_address->p_first;
	int curr_last = curr_details.entry_address->p_last;
	int curr_non = curr_details.entry_address->p_non_name;

	/* if the next word is in the dictionary and has a non-zero probability of
	being a last name we add the probability to next_last */
	int next_last = 0;
	if (next_details.entry_address) {
		next_last = next_details.entry_address->p_last;
	}

	/* attaches weighting to name probabilities given the context */
	if (next_last) {
		/* if the next word is likely to be a last name then the current word
		is more likely to be a first name */
		curr_first = curr_first + next_last;
	}
	/* first word is unlikely to be a last name so we weight it accordingly */
	curr_last = 0.5 * curr_last;

	/* assign nametype to most probably type after weighting */
	nametype = type_highest_prob(curr_first, curr_last, curr_non);

	return nametype;
}

/* calculates the type of the current word in context, this function is a 
combination of type_first_word and type_last_word */
int 
type_curr_word(word_details_t prev_details, word_details_t curr_details, 
	word_details_t next_details) {
	int nametype = 0;

	if (!curr_details.entry_address) {
		return nametype;
	}

	int curr_first = curr_details.entry_address->p_first;
	int curr_last = curr_details.entry_address->p_last;
	int curr_non = curr_details.entry_address->p_non_name;

	int next_last = 0;
	if (next_details.entry_address) {
		next_last = next_details.entry_address->p_last;
	}

	int prev_type = prev_details.nametype;

	/* attaches weighting to name probabilities given the context */
	if (next_last) {
		/* see type_first_word() */
		curr_first = curr_first + next_last;
	}

	if (prev_type == 1) {
		/* see type_last_word() */
		curr_last = curr_last * 2;
	}

	nametype = type_highest_prob(curr_first, curr_last, curr_non);

	return nametype;
}

int
type_last_word(word_details_t prev_details, word_details_t curr_details) {
	int nametype = 0;

	if (!curr_details.entry_address) {
		return nametype;
	}

	int curr_first = curr_details.entry_address->p_first;
	int curr_last = curr_details.entry_address->p_last;
	int curr_non = curr_details.entry_address->p_non_name;

	int prev_type = prev_details.nametype;

	/* attaches weighting to name probabilities given the context */
	if (prev_type == 1) {
		/* if the prev word is a first name the current word is much more
		likely to be a last name */
		curr_last = curr_last * 2;
	}

	nametype = type_highest_prob(curr_first, curr_last, curr_non);

	return nametype;	

}

/* given first, last and non. returns the type with highest probability */
int
type_highest_prob(int first, int last, int non) {
	if (first > last && first > non) {
		return 1;
	}
	else if (last > non) {
		return 2;
	}
	else {
		return 0;
	}
}

/* prints a name and its calculated type */
void 
print_smart_label(word_details_t curr_details) {
	printf("%-32s", curr_details.word);

	switch(curr_details.nametype) {
		case 0:
		printf("%s\n", NOT);
		break;

		case 1:
		printf("%s\n", FIRST);
		break;

		case 2:
		printf("%s\n", LAST);
		break;
	}
}