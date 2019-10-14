/* lab5-1.c */
/* Written by Alex Shved for Eltex Programing School, 10 oct 2019.
 * A library describes operations adding and subtraction.
 * */
 
#include <stdio.h>
#include <math.h>
#include <stdint.h>

double_t adding(const double_t base, const double_t part)
{
	return base + part;
}

uint8_t adding_uint8_t(const uint8_t base, const uint8_t part)
{
	return base + part;
}

double_t subtraction(const double_t base, const double_t part)
{
	return base - part;
}
