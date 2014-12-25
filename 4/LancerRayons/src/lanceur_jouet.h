#ifndef LANCEUR_JOUET_H
#define LANCEUR_JOUET_H

#define DEFAULT_TIME 1000

#define NO_STEAL 1
#define STEAL 0

typedef enum Distribution{
	UNIFORM, UNLUCKY
}Distribution;

extern Distribution __dist;
extern int __unlucky_proc;
extern int __numtasks;
extern int __time;
extern int * __tasktime;
extern int __steal_mode_jouet;

#endif