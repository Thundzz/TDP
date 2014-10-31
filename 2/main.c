#include "particule.h"


#define NBITER 10000

int main()
{
	pset *s = pset_alloc(1);
	pset *s2= pset_alloc(1);

	pset_init_rand(s);
	pset_init_rand(s2);

	pset_print(s);
	pset_print(s2);	

	for (int i = 0; i < NBITER ; ++i)
	{
		f_grav(s, s2);
		f_grav(s2, s);
		pset_step(s);
		pset_step(s2);
	}

	pset_print(s);
	pset_print(s2);	
	
	pset_free(s);
	pset_free(s2);
	return 0;
}