/* lab5-2.c */
/* Written by Alex Shved for Eltex Programing School, 10 oct 2019.
 * A library describes operations multiplication and division.
 * */

#include <stdio.h>
#include <math.h>

double_t multiplication(const double_t base, const double_t part)
{
	return base * part;
}

double_t division(const double_t base, const double_t part)
{
	if (part == 0)
	{
		perror("cannot be divided by null\n");
		return 0; // exit(1);
	}
	return base / part;
}
