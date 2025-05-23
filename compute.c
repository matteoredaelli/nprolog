#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include "npl.h"

//immediate small integer
int get_tag(int addr)
{
    if (addr >= 0 && addr < CELLSIZE)
	return (heap[addr].tag);
    else
	return (INTN);
}

int get_int(int addr)
{
    if (addr >= 0)
	return (INT_MASK & addr);
    else
	return (addr);
}

double get_flt(int x)
{
	return(GET_FLT(x));
}

//transform data type

int exact_to_inexact(int x)
{
    int res, tag;
    double val;

    tag = GET_TAG(x);
    switch (tag) {
    case INTN:
	res = freshcell();
	SET_TAG(res, FLTN);
	val = (double) GET_INT(x);
	SET_FLT(res, val);
	return (res);
    case LONGN:
	res = freshcell();
	SET_TAG(res, FLTN);
	val = (double) GET_LONG(x);
	SET_FLT(res, val);
	return (res);
    case BIGX:
	return (bigx_big_to_flt(x));
    case FLTN:
	return (x);
    default:
	return (x);
    }
    exception(SYSTEM_ERR, makestr("exact_to_inexact"), x, 0);
    return (UNDEF);
}

int numeqp(int x, int y)
{

    //if integer type and has same address, these are equal.
    if (integerp(x) && integerp(y)) {
	if (GET_INT(x) == GET_INT(y))
	    return (1);
	else
	    return (0);
    }
    //if long-integer type and tha values of these iare same, these are equal.
    else if (longnump(x) && longnump(y)) {
	if (GET_LONG(x) == GET_LONG(y))
	    return (1);
	else
	    return (0);
    }

    else if (bignump(x) && bignump(y)) {
	if (bigx_eqp(x, y))
	    return (1);
	else
	    return (0);
    }
    //for floating number. compare with considering EPSILON
    else if (floatp(x) && floatp(y)) {
      floatcomp:
	if (GET_FLT(x) - DBL_EPSILON <= GET_FLT(y) &&
	    GET_FLT(x) + DBL_EPSILON >= GET_FLT(y))
	    return (1);
	else if (GET_FLT(x) >= GET_FLT(y) - DBL_EPSILON &&
		 GET_FLT(x) <= GET_FLT(y) + DBL_EPSILON)
	    return (1);
	else
	    return (0);
    } else if (integerp(x) && floatp(y)) {
	x = exact_to_inexact(x);
	goto floatcomp;
    } else if (floatp(x) && integerp(y)) {
	y = exact_to_inexact(y);
	goto floatcomp;
    } else
	return (0);
}

int not_numeqp(int x, int y)
{

    //if integer type and has same address, these are equal.
    if (integerp(x) && integerp(y)) {
	if (GET_INT(x) != GET_INT(y))
	    return (1);
	else
	    return (0);
    }
    //if long-integer type and tha values of these iare same, these are equal.
    else if (longnump(x) && longnump(y)) {
	if (GET_LONG(x) != GET_LONG(y))
	    return (1);
	else
	    return (0);
    }

    else if (bignump(x) && bignump(y)) {
	if (!bigx_eqp(x, y))
	    return (1);
	else
	    return (0);
    }
    //for floating number. compare with considering EPSILON
    else if (floatp(x) && floatp(y)) {
      floatcomp:
	if (GET_FLT(x) - DBL_EPSILON <= GET_FLT(y) &&
	    GET_FLT(x) + DBL_EPSILON >= GET_FLT(y))
	    return (0);
	else if (GET_FLT(x) >= GET_FLT(y) - DBL_EPSILON &&
		 GET_FLT(x) <= GET_FLT(y) + DBL_EPSILON)
	    return (0);
	else
	    return (1);
    } else if ((integerp(x) || longnump(x) || bignump(x)) && floatp(y)) {
	x = exact_to_inexact(x);
	goto floatcomp;
    } else if (floatp(x) && (integerp(y) || longnump(y) || bignump(y))) {
	y = exact_to_inexact(y);
	goto floatcomp;
    } else
	return (1);
}

int smallerp(int x1, int x2)
{
    if (integerp(x1) && integerp(x2)) {
	if (GET_INT(x1) < GET_INT(x2))
	    return (1);
	else
	    return (0);
    } else if (integerp(x1) && bignump(x2))
	return (1);
    else if (longnump(x1) && bignump(x2))
	return (1);
    else if (longnump(x1) && longnump(x2)) {
	if (GET_LONG(x1) < GET_LONG(x2))
	    return (1);
	else
	    return (0);
    } else if (bignump(x1) && bignump(x2)) {
	if (bigx_smallerp(x1, x2))
	    return (1);
	else
	    return (0);
    } else {
	if (GET_FLT(exact_to_inexact(x1)) < GET_FLT(exact_to_inexact(x2)))
	    return (1);
	else
	    return (0);
    }
}

int eqsmallerp(int x1, int x2)
{
    if (integerp(x1) && integerp(x2)) {
	if (GET_INT(x1) <= GET_INT(x2))
	    return (1);
	else
	    return (0);
    } else if (integerp(x1) && bignump(x2))
	return (1);
    else if (longnump(x1) && bignump(x2))
	return (1);
    else if (longnump(x1) && longnump(x2)) {
	if (GET_LONG(x1) <= GET_LONG(x2))
	    return (1);
	else
	    return (0);
    } else if (bignump(x1) && bignump(x2)) {
	if (bigx_smallerp(x1, x2) || bigx_eqp(x1, x2))
	    return (1);
	else
	    return (0);
    } else {
	if (GET_FLT(exact_to_inexact(x1)) <= GET_FLT(exact_to_inexact(x2)))
	    return (1);
	else
	    return (0);
    }
}

int greaterp(int x1, int x2)
{
    if (smallerp(x2, x1))
	return (1);
    else
	return (0);
}

int eqgreaterp(int x1, int x2)
{
    if (eqsmallerp(x2, x1))
	return (1);
    else
	return (0);
}

int positivep(int x)
{

    if (integerp(x) && GET_INT(x) > 0)
	return (1);
    else if (longnump(x) && GET_LONG(x) > 0)
	return (1);
    else if (bignump(x) && bigx_positivep(x))
	return (1);
    else if (floatp(x) && GET_FLT(x) > 0)
	return (1);
    else
	return (0);
}

int negativep(int x)
{

    if (integerp(x) && GET_INT(x) < 0)
	return (1);
    else if (longnump(x) && GET_LONG(x) < 0)
	return (1);
    else if (bignump(x) && bigx_negativep(x))
	return (1);
    else if (floatp(x) && GET_FLT(x) < 0)
	return (1);
    else
	return (0);
}

int zerop(int x)
{
    if (integerp(x) && GET_INT(x) == 0)
	return (1);
    else if (floatp(x) && GET_FLT(x) == 0.0)
	return (1);
    else
	return (0);
}

int positive_zerop(int x)
{
    if (zerop(x) && floatp(x) && !signbit(GET_FLT(x)))
	return (1);
    else if (zerop(x) && floatp(x))
	return (1);
    else
	return (0);
}

int negative_zerop(int x)
{
    if (zerop(x) && floatp(x) && signbit(GET_FLT(x)))
	return (1);
    else
	return (0);
}


//-----------------------------------
//basic operation

int plus(int arg1, int arg2, int th)
{
    int n, s, tag1, tag2;
    double x1, y1, x2, result;
    long long int l;

    tag1 = GET_TAG(arg1);
    tag2 = GET_TAG(arg2);
    switch (tag1) {
    case INTN:
	switch (tag2) {
	case INTN:
	    {
		l = (long long int) GET_INT(arg1) +
		    (long long int) GET_INT(arg2);
		if (SMALL_INT_MIN < l && l < SMALL_INT_MAX)
		    return (makeint((int) l));
		else
		    return (makelong(l));
	    }
	case FLTN:
	    {
		n = GET_INT(arg1);
		x1 = (double) n;
		y1 = GET_FLT(arg2);
		result = x1 + y1;
		if (isinf(result))
		    return (ERROBJ);
		return (makeflt(result));
	    }
	case LONGN:
	    return (bigx_plus
		    (bigx_int_to_big(arg1), bigx_long_to_big(arg2)));
	case BIGX:
	    return (bigx_plus(bigx_int_to_big(arg1), arg2));
	}
    case LONGN:
	switch (tag2) {
	case INTN:
	    return (bigx_plus
		    (bigx_long_to_big(arg1), bigx_int_to_big(arg2)));
	case FLTN:
	    return (plus(exact_to_inexact(arg1), arg2, th));
	case LONGN:
	    return (bigx_plus
		    (bigx_long_to_big(arg1), bigx_long_to_big(arg2)));
	case BIGX:
	    return (bigx_plus(bigx_long_to_big(arg1), arg2));
	}
    case BIGX:
	switch (tag2) {
	case INTN:
	    return (bigx_plus(arg1, bigx_int_to_big(arg2)));
	case FLTN:
	    return (plus(exact_to_inexact(arg1), arg2, th));
	case LONGN:
	    return (bigx_plus(arg1, bigx_long_to_big(arg2)));
	case BIGX:
	    return (bigx_plus(arg1, arg2));
	}
    case FLTN:
	switch (tag2) {
	case INTN:
	    {
		x1 = GET_FLT(arg1);
		s = GET_INT(arg2);
		x2 = (double) s;
		result = x1 + x2;
		if (isinf(result))
		    return (ERROBJ);
		return (makeflt(result));
	    }
	case FLTN:
	    {
		x1 = GET_FLT(arg1);
		x2 = GET_FLT(arg2);
		result = x1 + x2;
		if (isinf(result))
		    return (ERROBJ);
		return (makeflt(result));
	    }
	case LONGN:
	    return (plus(arg1, exact_to_inexact(arg2), th));
	case BIGX:
	    return (plus(arg1, exact_to_inexact(arg2), th));

	}
    }

    return (UNDEF);
}


int minus(int arg1, int arg2, int th)
{
    int n, s, tag1, tag2;
    double x1, y1, x2;
    long long int l;

    tag1 = GET_TAG(arg1);
    tag2 = GET_TAG(arg2);
    switch (tag1) {
    case INTN:
	switch (tag2) {
	case INTN:
	    {
		l = (long long int) GET_INT(arg1) -
		    (long long int) GET_INT(arg2);
		if (SMALL_INT_MIN < l && l < SMALL_INT_MAX)
		    return (makeint((int) l));
		else
		    return (makelong(l));
	    }

	case FLTN:
	    {
		n = GET_INT(arg1);
		x1 = (double) n;
		y1 = GET_FLT(arg2);
		return (makeflt(x1 - y1));
	    }

	case LONGN:
	    return (bigx_minus(bigx_int_to_big(arg1),
			       bigx_long_to_big(arg2)));

	case BIGX:
	    return (bigx_minus(bigx_int_to_big(arg1), arg2));
	}
    case LONGN:
	switch (tag2) {
	case INTN:
	    return (bigx_minus
		    (bigx_long_to_big(arg1), bigx_int_to_big(arg2)));
	case FLTN:
	    return (minus(exact_to_inexact(arg1), arg2, th));
	case LONGN:
	    return (bigx_minus
		    (bigx_long_to_big(arg1), bigx_long_to_big(arg2)));
	case BIGX:
	    return (bigx_minus(bigx_long_to_big(arg1), arg2));
	}

    case BIGX:
	switch (tag2) {
	case INTN:
	    return (bigx_minus(arg1, bigx_int_to_big(arg2)));
	case FLTN:
	    return (minus(exact_to_inexact(arg1), arg2, th));
	case LONGN:
	    return (bigx_minus(arg1, bigx_long_to_big(arg2)));
	case BIGX:
	    return (bigx_minus(arg1, arg2));
	}

    case FLTN:
	switch (tag2) {
	case INTN:
	    {
		x1 = GET_FLT(arg1);
		s = GET_INT(arg2);
		x2 = (double) s;
		return (makeflt(x1 - x2));
	    }
	case FLTN:
	    {
		x1 = GET_FLT(arg1);
		x2 = GET_FLT(arg2);
		return (makeflt(x1 - x2));
	    }
	case LONGN:
	    return (minus(arg1, exact_to_inexact(arg2), th));
	case BIGX:
	    return (minus(arg1, exact_to_inexact(arg2), th));
	}
    }

    return (UNDEF);
}


int mult(int arg1, int arg2, int th)
{
    int n, s, tag1, tag2;
    long long int l, l1, l2;
    double x1, y1, x2;

    tag1 = GET_TAG(arg1);
    tag2 = GET_TAG(arg2);

    switch (tag1) {
    case INTN:
	switch (tag2) {
	case INTN:
	    {
		l1 = (long long int) GET_INT(arg1);
		l2 = (long long int) GET_INT(arg2);
		l = l1 * l2;
		if (l < SMALL_INT_MAX && l > SMALL_INT_MIN)
		    return (makeint((int) l));
		else
		    return (bigx_mult
			    (bigx_int_to_big(arg1),
			     bigx_int_to_big(arg2)));
	    }

	case FLTN:
	    {
		n = GET_INT(arg1);
		x1 = (double) n;
		y1 = GET_FLT(arg2);
		return (makeflt(x1 * y1));
	    }

	case LONGN:
	    if (GET_INT(arg1) != 0)
		return (bigx_mult
			(bigx_int_to_big(arg1), bigx_long_to_big(arg2)));
	    else
		return (arg1);	//int 0

	case BIGX:
	    return (bigx_mult_i(arg2, arg1));
	}

    case LONGN:
	switch (tag2) {
	case INTN:
	    if (GET_INT(arg2) != 0)
		return (bigx_mult
			(bigx_long_to_big(arg1), bigx_int_to_big(arg2)));
	    else
		return (arg2);	//int 0
	case FLTN:
	    return (mult(exact_to_inexact(arg1), arg2, th));
	case LONGN:
	    return (bigx_mult
		    (bigx_long_to_big(arg1), bigx_long_to_big(arg2)));
	case BIGX:
	    return (bigx_mult(bigx_long_to_big(arg1), arg2));
	}

    case BIGX:
	switch (tag2) {
	case INTN:
	    return (bigx_mult_i(arg1, arg2));
	case FLTN:
	    return (mult(exact_to_inexact(arg1), arg2, th));
	case LONGN:
	    return (bigx_mult(arg1, bigx_long_to_big(arg2)));
	case BIGX:
	    return (bigx_mult(arg1, arg2));
	}

    case FLTN:
	switch (tag2) {
	case INTN:
	    {
		x1 = GET_FLT(arg1);
		s = GET_INT(arg2);
		x2 = (double) s;
		return (makeflt(x1 * x2));
	    }
	case FLTN:
	    {
		x1 = GET_FLT(arg1);
		x2 = GET_FLT(arg2);
		y1 = x1 * x2;
		if (y1 > DBL_MAX || y1 < -DBL_MAX)
		    return (ERROBJ);
		if (x1 != 0.0 && x2 != 0.0 && y1 > -DBL_MIN
		    && y1 < DBL_MIN)
		    return (ERROBJ);
		return (makeflt(y1));
	    }
	case LONGN:
	case BIGX:
	    return (mult(arg1, exact_to_inexact(arg2), th));
	}

    }

    return (UNDEF);
}

int divide(int arg1, int arg2, int th)
{
    int n, s, tag1, tag2;
    double x1, y1, x2;

    tag1 = GET_TAG(arg1);
    tag2 = GET_TAG(arg2);
    switch (tag1) {
    case INTN:
	switch (tag2) {
	case INTN:
	    {
		n = GET_INT(arg1);
		x1 = (double) n;
		s = GET_INT(arg2);
		y1 = (double) s;
		x2 = x1 / y1;
		if ((x2 - floor(x2)) == 0.0)
		    return (makeint((int) x2));
		else
		    return (makeflt(x1 / y1));
	    }
	case FLTN:
	    {
		n = GET_INT(arg1);
		x1 = (double) n;
		y1 = GET_FLT(arg2);
		return (makeflt(x1 / y1));
	    }

	case LONGN:
	case BIGX:
	    return (divide
		    (exact_to_inexact(arg1), exact_to_inexact(arg2), th));
	}
    case LONGN:
	switch (tag2) {
	case INTN:
	case LONGN:
	case BIGX:
	    return (divide
		    (exact_to_inexact(arg1), exact_to_inexact(arg2), th));
	case FLTN:
	    return (divide(exact_to_inexact(arg1), arg2, th));
	}

    case BIGX:
	switch (tag2) {
	case INTN:
	case LONGN:
	case BIGX:
	    return (divide
		    (exact_to_inexact(arg1), exact_to_inexact(arg2), th));
	case FLTN:
	    return (divide(exact_to_inexact(arg1), arg2, th));
	}
    case FLTN:
	switch (tag2) {
	case INTN:
	    {
		x1 = GET_FLT(arg1);
		s = GET_INT(arg2);
		x2 = (double) s;
		return (makeflt(x1 / x2));
	    }
	case FLTN:
	    {
		x1 = GET_FLT(arg1);
		x2 = GET_FLT(arg2);
		return (makeflt(x1 / x2));
	    }
	case LONGN:
	case BIGX:
	    return (divide(arg1, exact_to_inexact(arg2), th));

	}
    }

    return (UNDEF);
}


int quotient(int x, int y, int th)
{

    if (integerp(x) && longnump(y))
	return (makeint(0));
    else if (integerp(x) && bignump(y))
	return (makeint(0));
    else if (longnump(x) && bignump(y))
	return (makeint(0));
    else if (integerp(x) && integerp(y))
	return (makeint(GET_INT(x) / GET_INT(y)));
    else if (longnump(x) && integerp(y))
	return (long_int_quotient(x, y));
    else if (longnump(x) && longnump(y))
	return (long_long_quotient(x, y));
    else if (bignump(x) && integerp(y))
	return (bigx_div_i(x, y));
    else if (bignump(x) && longnump(y))
	return (bigx_div(x, bigx_long_to_big(y)));
    else if (bignump(x) && bignump(y))
	return (bigx_div(x, y));
    else
	return (UNDEF);
}

int s_remainder(int x, int y, int th)
{
    int i;

    if (integerp(x) && integerp(y))
	return (makeint(GET_INT(x) % GET_INT(y)));
    else if (integerp(x) && longnump(y))
	return (x);
    else if (integerp(x) && bignump(y))
	return (x);
    else if (longnump(x) && bignump(y))
	return (x);
    else if (longnump(x) && integerp(y))
	return (long_int_remainder(x, y));
    else if (longnump(x) && longnump(y))
	return (long_long_remainder(x, y));
    else if (bignump(x) && integerp(y))
	return (bigx_remainder_i(x, y));
    else if (bignump(x) && longnump(y)) {
	i = bigx_long_to_big(y);
	return (minus(x, mult(quotient(x, i, th), i, th), th));
    } else if (bignump(x) && bignump(y))
	return (minus(x, mult(quotient(x, y, th), y, th), th));

    return (UNDEF);
}


//remainder of  longnum and int
int long_int_remainder(int x, int y)
{
    long long int m, n, r;


    m = GET_LONG(x);
    n = (long long int) GET_INT(y);
    r = m % n;

    return (makeint((int) r));
}

//remainder of longnum and longnum
int long_long_remainder(int x, int y)
{
    long long int m, n, r;


    m = GET_LONG(x);
    n = GET_LONG(y);
    r = m % n;

    if (llabs(r) < BIGNUM_BASE)
	return (makeint((int) r));
    else
	return (makelong(r));
}

//quotient of longnum and int
int long_int_quotient(int x, int y)
{
    long long int m, n, q;


    m = GET_LONG(x);
    n = (long long int) GET_INT(y);
    q = m / n;

    if (llabs(q) < BIGNUM_BASE)
	return (makeint((int) q));
    else
	return (makelong(q));
}

//quotient of longnum and longnum
int long_long_quotient(int x, int y)
{
    long long int m, n, q;


    m = GET_LONG(x);
    n = GET_LONG(y);
    q = m / n;

    if (llabs(q) < BIGNUM_BASE)
	return (makeint((int) q));
    else
	return (makelong(q));
}

int absolute(int x, int th)
{
    if (integerp(x))
	return (makeint(abs(GET_INT(x))));
    else if (longnump(x))
	return (makelong(llabs(GET_LONG(x))));
    else if (bignump(x)) {
	return (bigx_abs(x));
    } else if (floatp(x)) {
	return (makeflt(fabs(GET_FLT(x))));
    }
    return (UNDEF);
}
