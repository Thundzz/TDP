#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "lanceur_jouet.h"
#include "img.h"

/////////////////////////////
// Pour eviter des erreurs de linkage... Très moche...
// Tout ce qui est déclaré ici est toute façon inutilisé dans la version jouet
void assign_epsilon (void){}
REAL     Epsilon;
VECTOR   Void,Half,Full;
COLOR    White,Black;
REAL     Epsilon;
INDEX    NbrLevel;
/////////////////////////////

int main (int argc, char **argv)
{
	__dist = UNIFORM;
	__time = DEFAULT_TIME;
	__steal_mode_jouet = STEAL;
	int set_numtasks = 0;

	int             c;
	const char    * short_opt = "bn:N:t:hs";
	struct option   long_opt[] =
	{
		{"bad",          no_argument, NULL, 'b'},
		{"numstasks",         required_argument, NULL, 'n'},
		{"numbtasks",         required_argument, NULL, 'n'},
		{"time",         required_argument, NULL, 't'},	
		{"nosteal",         no_argument, NULL, 's'},
		{"help",          	 no_argument, NULL, 'h'},
		{NULL,            0,                 NULL, 0  }
	};

	while((c = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1)
	{
		switch(c)
		{
			case -1:       /* no more arguments */
			case 0:        /* long options toggles */
			break;

			case 'b':
			__dist = BAD;
			break;

			case 'n':
			set_numtasks += 1;
			__numstasks = atoi(optarg);
			break;		
			case 'N':
			set_numtasks += 1;
			__numbtasks = atoi(optarg);
			break;

			case 't':
			__time = atof(optarg);
			break;

			case 's':
			__steal_mode_jouet = NO_STEAL;
			break;

			case 'h':
			printf("Usage: %s -n 'number_of_small_tasks' -N 'number_of_big_tasks' [OPTIONS]\n", argv[0]);
			printf("OPTIONS: \n");
			printf("  -b, --bad     			bad distribution of tasks\n");
			printf("  -t, --time  [ARGUMENT]    time of a small task, in usec" 
												"(default: 1000 usec)\n");			
			printf("  -s, --nosteal    			no task steal version \n");
			printf("  -h, --help        		print this help and exit\n");
			printf("\n");
			return(0);

			case ':':
			case '?':
			fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
			return(-2);

			default:
			fprintf(stderr, "%s: invalid option -- %c\n", argv[0], c);
			fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
			return(-2);
		};
	};

	if(set_numtasks < 2)
	{
		fprintf(stderr, "You must set the number of small and big tasks.\n");
		fprintf(stderr, "Try `%s --help' for more information.\n", argv[0]);
		return -2;
	}
	else
	{
		printf("Number of small tasks : %d\n", __numbtasks);
		printf("Number of big tasks: %d\n",	__numstasks);
		printf("Distribution : %d\n", __dist);
		printf("Task time : %d\n", __time);
		if(__steal_mode_jouet == NO_STEAL)
		{
			printf("No-steal mode activated. Tasks distribution will be static\n");
		}
	}
	img (NULL);
	return 0;
}