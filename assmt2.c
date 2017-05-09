/* Programmer: Andy Vo, voa1@student.unimelb.edu.au

   Algorithms are fun!
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define MAX_NAME_LENGTH 30 /* max chars in a name */
#define MAX_DICT_LENGTH 100 /* max entries in dictionary */
#define MAX_LABEL_LENGTH 10 /* max chars in name labels */

typedef char word_t[MAX_NAME_LENGTH];
#include "listops.c"

#define DIV "=========================" /* stage header */

/* stage numbers */
#define STAGE_NUM_ONE 1 
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_NUM_FIVE 5

/* struct to store properties of an entry in the dict */
typedef struct namedict_t{
	char name_ref[MAX_NAME_LENGTH];
	int p_first;
	int p_last;
	int p_non_name;
} namedict_t;

/* function prototypes */
int read_namedict(namedict_t namedict[]);
int read_sentence(list_t *sentence);
void print_stage_header(int stage_no);
void stage_one (namedict_t namedict[]);
void print_word (namedict_t namedict[], int word_no);
void stage_two(namedict_t namedict[], int dict_length);
void stage_three(list_t *sentence);
void stage_four(namedict_t namedict[], int dict_length, list_t *sentence, 
	int sentence_length);

/* main program */
int
main(int argc, char *argv[]) {
	namedict_t namedict[MAX_DICT_LENGTH]; /* stores dictionary entries */
	list_t *sentence = make_empty_list(); /* stores words from sentence */
	int dict_length;
	int sentence_length;

	/* reads our dictionary entries into namedict*/
	dict_length = read_namedict(namedict);

	/* reads words into sentence */
	sentence_length = read_sentence(sentence);

	/* stage 1 */
	stage_one(namedict);
	
	/* stage 2 */
	stage_two(namedict, dict_length);

	/* stage 3 */
	stage_three(sentence);

	/* stage 4 */
	stage_four(namedict, dict_length, sentence, sentence_length);

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

		/* individually reads in suffix before reading in name */
		scanf("%c%s", &hash, curr_word);

		/* breaks the loop when the end of the dict is reached */
		if (hash == '%') {
			break;
		}

		/* copies curr_word into our dictionary array, copying after the break
		condition rather than in the scanf function prevents the %%%%%%%%%% 
		line from being copied into our array */
		strcpy (namedict[i].name_ref, curr_word);

		/* reads in probabilities */
		scanf("%d %d %d\n", &namedict[i].p_first, &namedict[i].p_last, 
			&namedict[i].p_non_name);
	}
	dict_length = i;
	return dict_length;
}

/* reads words into our sentence linked list */
int read_sentence(list_t *sentence) {
	int sentence_length = 0;
	word_t curr_word;
	while (scanf("%s", curr_word) != EOF) {
		insert_at_foot(sentence, curr_word);
		sentence_length++;
	}
	return sentence_length;
}

/* prints a stage header */
void
print_stage_header(int stage_no) {
	if (stage_no > STAGE_NUM_ONE) {
		printf("\n");
	}
	printf("%sStage %d%s\n", DIV, stage_no, DIV);
}

/* prints the first word from our dictionary */
void
stage_one (namedict_t namedict[]){
	print_stage_header(STAGE_NUM_ONE);
	print_word(namedict, 0);
}

/* prints a single word, seperated from stage 1 so it can be called for
debugging */
void
print_word (namedict_t namedict[], int word_no) {
	printf("Word %d: %s\n", word_no, namedict[word_no].name_ref);
	printf("Label probabilities: %d%% %d%% %d%%\n", namedict[word_no].p_first, 
		namedict[word_no].p_last, namedict[word_no].p_non_name);
}

/* calculates and prints the average character per word in namedict */
void
stage_two(namedict_t namedict[], int dict_length) {
	print_stage_header(STAGE_NUM_TWO);

	int char_count = 0;
	double avg_count;
	int i, j;

	/* counts total characters in all names and divides to get the mean */
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
	while(sentence->head) {
		strcpy(curr_word, get_head(sentence));
		printf("%s\n", curr_word);
		sentence = get_tail(sentence);
	}
}

/*
void
stage_three(list_t *sentence) {
	print_stage_header(STAGE_NUM_THREE);

	printf("%s\n", sentence->head->data);
	word_t curr_word;
	while(sentence->head->next) {
		strcpy(curr_word, sentence->head->data);
		printf("%s\n", curr_word);
	}
}
*/

/* searches namedict for each word in sentence, if the word is present we print
FIRST_NAME/LAST_NAME if it has non-zero values for p_first and/or p_last */
void
stage_four(namedict_t namedict[], int dict_length, list_t *sentence,
	int sentence_length) {
	print_stage_header(STAGE_NUM_FOUR);

	char first[MAX_LABEL_LENGTH] = "FIRST_NAME";
	char last[MAX_LABEL_LENGTH] = "LAST_NAME";
	char not[MAX_LABEL_LENGTH] = "NOT_NAME";

	word_t curr_word;
	namedict_t* name_address;

	while(sentence->head) {
		strcpy(curr_word, get_head(sentence));
		sentence = get_tail(sentence);

		/* use a binary search to find curr_word in namedict
		we are able to search for a string within a stuct since the string is
		the first variable in the struct and strcmp conviently stops when \0
		is encountered */
		name_address = bsearch(&curr_word, namedict, dict_length, 
			sizeof (namedict_t), (int(*)(const void*,const void*)) strcmp);

		/* this conditional takes advantage of the fact that bsearch returns 
		null if no match is found */
		if (name_address) {
			printf("%-32s", curr_word);
			if (name_address->p_first && name_address->p_last) {
				printf("%s, %s\n", first, last);
			}
			else if (name_address->p_first) {
				printf("%s\n", first);
			}
			else if (name_address->p_last) {
				printf("%s\n", last);
			}
			else {
				printf("%s\n", not);
			}
		}
		else {
			printf("%-32s", curr_word);
			printf("%s\n", not);
		}
	}
}