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
	const char    * short_opt = "ul:n:t:hs";
	struct option   long_opt[] =
	{
		{"uniform",          no_argument, NULL, 'u'},
		{"unlucky",          required_argument, NULL, 'l'},
		{"numtasks",         required_argument, NULL, 'n'},
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

			case 'u':
			break;

			case 'l':
			__dist = UNLUCKY;
			__unlucky_proc = atoi(optarg);
			break;

			case 'n':
			set_numtasks = 1;
			__numtasks = atoi(optarg);
			break;

			case 't':
			__time = atof(optarg);
			break;

			case 's':
			__steal_mode_jouet = NO_STEAL;
			break;

			case 'h':
			printf("Usage: %s -n 'number_of_tasks' [OPTIONS]\n", argv[0]);
			printf("OPTIONS: \n");
			printf("  -u, --uniform  (default)    	uniform distribution of tasks\n");
			printf("  -l, --unlucky  [ARGUMENT]    	one designated process" 
													"is given 10 times longer tasks\n");
			printf("  -t, --time  [ARGUMENT]    time of a task, in usec" 
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

	if(!set_numtasks)
	{
		fprintf(stderr, "You must set the number of tasks. Try `%s --help' for more information.\n", argv[0]);
		return -2;
	}
	else
	{
		printf("Number of tasks : %d\n", __numtasks);
		printf("Distribution : %d\n", __dist);
		printf("Task time : %d\n", __time);
		if(__dist == UNLUCKY)
		{
			printf("Unlucky process : %d\n", __unlucky_proc);
			printf("Long tasks time : %d\n", __time*10);
		}
		if(__steal_mode_jouet == NO_STEAL)
		{
			printf("No-steal mode activated. Tasks distribution will be static\n");
		}
	}
	img (NULL);
	return 0;
}