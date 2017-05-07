/* Programmer: Andy Vo, voa1@student.unimelb.edu.au

   Algorithms are fun!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 30
#define MAX_DICT_LENGTH 100

typedef struct namedict_t{
	char name_ref[MAX_NAME_LENGTH];
	int p_first;
	int p_last;
	int p_non_name;
} namedict_t;

/* function prototypes */
namedict_t read_dict();

/* main program binds it all together */
int
main(int argc, char *argv[]) {
	namedict_t namedict[MAX_DICT_LENGTH];
	
	*namedict = read_dict();
	return 0;
}

namedict_t
read_dict() {
	namedict_t namedict[MAX_DICT_LENGTH];
	char tempname[MAX_NAME_LENGTH];
	char hash;

	int i;
	for (i = 0;; i++) {

		scanf("%c%s", &hash, tempname);
		
		if (hash == '%') {
			break;
		}

		strcpy (namedict[i].name_ref, tempname);

		scanf("%d %d %d\n", &namedict[i].p_first, &namedict[i].p_last, 
			&namedict[i].p_non_name);
	}
	
	return *namedict;
}