//////////////////////////////////////////////////////////////////////////
//
//  Fixed Point Math Class
//
//////////////////////////////////////////////////////////////////////////
//
//  Released under GNU license
//		Erik H Gawtry
//			July, 2005	Version 1.0
//
//
//  Algorythms borrowed from:
//		Andrew Ryder, 11 September 2001
//      Joseph Hall, Unknown Date
//
//
//////////////////////////////////////////////////////////////////////////
//
// Written for doing FixedPoint point math on DSP processors
//
//////////////////////////////////////////////////////////////////////////

#include "fixed.h"
#include <math.h>

#define	RESOLUTION			10000000L
#define	RESOLUTION_FLOAT	10000000.0
#define	RESOLUTION_FLOATf	10000000.0f
#define	FLOAT_RESOLUTION	0.00000005
#define	FLOAT_RESOLUTIONf	0.00000005f
#define _XPI      31415920 // 3.1415926535897932384626433832795
#define XPI      FixedPoint(true,_XPI)
#define _X2PI     62831850 // 6.283185307179586476925286766559
#define X2PI     FixedPoint(true,_X2PI)
#define _XPIO2    15707960 // 1.5707963267948966192313216916398
#define XPIO2    FixedPoint(true,_XPIO2)
#define _XPIO4    7853980 // 0.78539816339744830961566084581988
#define XPIO4    FixedPoint(true, _XPIO4)
#define _XLN_E    27182820 // 2.71828182845904523536
#define XLN_E    FixedPoint(true,_XLN_E)
#define _XLN_10   23025850 // 2.30258509299404568402
#define XLN_10   FixedPoint(true,_XLN_10)
#define sqrt_error   FixedPoint(true, 1000) // 0.001

FixedPoint::FixedPoint(void)
{
	m_nVal = 0;
}

FixedPoint::FixedPoint(const FixedPoint& fixedVal)
{
	m_nVal = fixedVal.m_nVal;
}

FixedPoint::FixedPoint(const FixedPoint* fixedVal)
{
	m_nVal = fixedVal->m_nVal;
}

FixedPoint::FixedPoint(bool bInternal, long nVal)
{
	m_nVal = nVal;
}

FixedPoint::FixedPoint(long nVal)
{
	m_nVal = nVal*RESOLUTION;
}

FixedPoint::FixedPoint(long long nVal)
{
	m_nVal = nVal*RESOLUTION;
}

FixedPoint::FixedPoint(int nVal)
{
	m_nVal = nVal*RESOLUTION;
}

FixedPoint::FixedPoint(short nVal)
{
	m_nVal = nVal*RESOLUTION;
}

#ifdef FIXED_HAS_DOUBLE
FixedPoint::FixedPoint(double floatVal)
{
	floatVal += FLOAT_RESOLUTION;
	m_nVal = (long)::floor(floatVal*RESOLUTION);
}
#endif

FixedPoint::FixedPoint(float floatVal)
{
	floatVal += FLOAT_RESOLUTIONf;
	m_nVal = (long)::floorf(floatVal*RESOLUTION);
}

FixedPoint::~FixedPoint(void)
{
}

FixedPoint FixedPoint::operator++(void)
{
	m_nVal += RESOLUTION;
	return *this;
}

FixedPoint FixedPoint::operator--(void)
{
	m_nVal -= RESOLUTION;
	return *this;
}

FixedPoint& FixedPoint::operator=(FixedPoint fixedVal)
{
	m_nVal = fixedVal.m_nVal;
	return *this;
}

FixedPoint& FixedPoint::operator=(float floatVal)
{
	floatVal += FLOAT_RESOLUTIONf;
	m_nVal = (long)::floorf(floatVal*RESOLUTION);
	return *this;
}

#ifdef FIXED_HAS_DOUBLE
FixedPoint& FixedPoint::operator=(double floatVal)
{
	floatVal+=FLOAT_RESOLUTION;
	m_nVal = (long)::floor(floatVal*RESOLUTION);
	return *this;
}
#endif

FixedPoint& FixedPoint::operator=(int intVal)
{
	m_nVal = intVal*RESOLUTION;
	return *this;
}

FixedPoint& FixedPoint::operator=(unsigned int intVal)
{
	m_nVal = intVal*RESOLUTION;
	return *this;
}

#ifdef FIXED_HAS_LONG
FixedPoint& FixedPoint::operator=(long longVal)
{
	m_nVal = longVal*RESOLUTION;
	return *this;
}

FixedPoint& FixedPoint::operator=(unsigned long longVal)
{
	m_nVal = longVal*RESOLUTION;
	return *this;
}
#endif

FixedPoint& FixedPoint::operator=(short shortVal)
{
	m_nVal = shortVal*RESOLUTION;
	return *this;
}

FixedPoint& FixedPoint::operator=(unsigned short shortVal)
{
	m_nVal = shortVal*RESOLUTION;
	return *this;
}

FixedPoint& FixedPoint::operator=(char charVal)
{
	m_nVal = charVal*RESOLUTION;
	return *this;
}

FixedPoint& FixedPoint::operator=(unsigned char charVal)
{
	m_nVal = charVal*RESOLUTION;
	return *this;
}

bool FixedPoint::operator==(float floatVal)
{
	floatVal+=FLOAT_RESOLUTIONf;
	return (m_nVal == (long)::floorf(floatVal*RESOLUTION));
}

#ifdef FIXED_HAS_DOUBLE
bool FixedPoint::operator==(double floatVal)
{
	floatVal+=FLOAT_RESOLUTION;
	return (m_nVal == (long)::floor(floatVal*RESOLUTION));
}
#endif

bool FixedPoint::operator==(FixedPoint fixedVal)
{
	return (m_nVal == fixedVal.m_nVal);
}

bool FixedPoint::operator==(int intVal)
{
	return (m_nVal == intVal*RESOLUTION);
}

#ifdef FIXED_HAS_LONG
bool FixedPoint::operator==(long intVal)
{
	return (m_nVal == intVal*RESOLUTION);
}
#endif

bool FixedPoint::operator==(short intVal)
{
	return (m_nVal == intVal*RESOLUTION);
}

bool FixedPoint::lessthan(FixedPoint b)
{
	return (m_nVal < b.m_nVal);
}

bool FixedPoint::lessthanequal(FixedPoint b)
{
	return (m_nVal <= b.m_nVal);
}

bool FixedPoint::operator<(float floatVal)
{
	floatVal+=FLOAT_RESOLUTIONf;
	return (m_nVal < (long)::floorf(floatVal*RESOLUTION));
}

#ifdef FIXED_HAS_DOUBLE
bool FixedPoint::operator<(double floatVal)
{
	floatVal+=FLOAT_RESOLUTION;
	return (m_nVal < (long)::floor(floatVal*RESOLUTION));
}
#endif

bool FixedPoint::operator<(FixedPoint fixedVal)
{
	return (m_nVal < fixedVal.m_nVal);
}

bool FixedPoint::operator<(int intVal)
{
	return (m_nVal < intVal*RESOLUTION);
}

#ifdef FIXED_HAS_LONG
bool FixedPoint::operator<(long intVal)
{
	return (m_nVal < intVal*RESOLUTION);
}
#endif

bool FixedPoint::operator<(short intVal)
{
	return (m_nVal < intVal*RESOLUTION);
}

bool FixedPoint::operator<=(float floatVal)
{
	floatVal+=FLOAT_RESOLUTIONf;
	return (m_nVal <= (long)::floorf(floatVal*RESOLUTION));
}

#ifdef FIXED_HAS_DOUBLE
bool FixedPoint::operator<=(double floatVal)
{
	floatVal+=FLOAT_RESOLUTION;
	return (m_nVal <= (long)::floor(floatVal*RESOLUTION));
}
#endif

bool FixedPoint::operator<=(FixedPoint fixedVal)
{
	return (m_nVal <= fixedVal.m_nVal);
}

bool FixedPoint::operator<=(int intVal)
{
	return (m_nVal <= intVal*RESOLUTION);
}

#ifdef FIXED_HAS_LONG
bool FixedPoint::operator<=(long intVal)
{
	return (m_nVal <= intVal*RESOLUTION);
}
#endif

bool FixedPoint::operator<=(short intVal)
{
	return (m_nVal <= intVal*RESOLUTION);
}

bool FixedPoint::operator>(float floatVal)
{
	floatVal+=FLOAT_RESOLUTIONf;
	return (m_nVal > (long)::floorf(floatVal*RESOLUTION));
}

#ifdef FIXED_HAS_DOUBLE
bool FixedPoint::operator>(double floatVal)
{
	floatVal+=FLOAT_RESOLUTION;
	return (m_nVal > (long)::floor(floatVal*RESOLUTION));
}
#endif

bool FixedPoint::operator>(FixedPoint fixedVal)
{
	return (m_nVal > fixedVal.m_nVal);
}

bool FixedPoint::operator>(int intVal)
{
	return (m_nVal > intVal*RESOLUTION);
}

#ifdef FIXED_HAS_LONG
bool FixedPoint::operator>(long intVal)
{
	return (m_nVal > intVal*RESOLUTION);
}
#endif

bool FixedPoint::operator>(short intVal)
{
	return (m_nVal > intVal*RESOLUTION);
}

bool FixedPoint::operator>=(float floatVal)
{
	floatVal+=FLOAT_RESOLUTIONf;
	return (m_nVal >= (long)::floorf(floatVal*RESOLUTION));
}

#ifdef FIXED_HAS_DOUBLE
bool FixedPoint::operator>=(double floatVal)
{
	floatVal+=FLOAT_RESOLUTION;
	return (m_nVal >= (long)::floor(floatVal*RESOLUTION));
}
#endif

bool FixedPoint::operator>=(FixedPoint fixedVal)
{
	return (m_nVal >= fixedVal.m_nVal);
}

bool FixedPoint::operator>=(int intVal)
{
	return (m_nVal >= intVal*RESOLUTION);
}

#ifdef FIXED_HAS_LONG
bool FixedPoint::operator>=(long intVal)
{
	return (m_nVal >= intVal*RESOLUTION);
}
#endif

bool FixedPoint::operator>=(short intVal)
{
	return (m_nVal >= intVal*RESOLUTION);
}

bool FixedPoint::operator!=(float floatVal)
{
	floatVal+=FLOAT_RESOLUTIONf;
	return (m_nVal != (long)::floorf(floatVal*RESOLUTION));
}

#ifdef FIXED_HAS_DOUBLE
bool FixedPoint::operator!=(double floatVal)
{
	floatVal+=FLOAT_RESOLUTION;
	return (m_nVal != (long)::floor(floatVal*RESOLUTION));
}
#endif

bool FixedPoint::operator!=(FixedPoint fixedVal)
{
	return (m_nVal != fixedVal.m_nVal);
}

bool FixedPoint::operator!=(int intVal)
{
	return (m_nVal != intVal*RESOLUTION);
}

#ifdef FIXED_HAS_LONG
bool FixedPoint::operator!=(long intVal)
{
	return (m_nVal != intVal*RESOLUTION);
}
#endif

bool FixedPoint::operator!=(short intVal)
{
	return (m_nVal != intVal*RESOLUTION);
}

FixedPoint::operator float(void)
{
	return m_nVal/RESOLUTION_FLOATf;
}

#ifdef FIXED_HAS_DOUBLE
FixedPoint::operator double(void)
{
	return m_nVal/RESOLUTION_FLOAT;
}
#endif

#ifdef FIXED_HAS_LONG
long FixedPoint::GetLong(void)
{
	return m_nVal/RESOLUTION;
}

FixedPoint::operator long(void)
{
	return (m_nVal/RESOLUTION);
}
#endif

FixedPoint::operator int(void)
{
	return (int)(m_nVal/RESOLUTION);
}

FixedPoint::operator short(void)
{
	return (short)(m_nVal/RESOLUTION);
}

FixedPoint::operator unsigned short(void)
{
	return (unsigned short)(m_nVal/RESOLUTION);
}

FixedPoint FixedPoint::floor(void)
{
	return (FixedPoint)(m_nVal/RESOLUTION);
}

FixedPoint FixedPoint::ceil(void)
{
	return (FixedPoint)(m_nVal/RESOLUTION+1);
}

FixedPoint FixedPoint::operator%(FixedPoint fixedVal)
{
	FixedPoint a;
	a.m_nVal = m_nVal%fixedVal.m_nVal;
	return a;
}

#ifdef FIXED_HAS_LONG
FixedPoint FixedPoint::operator%(long longVal)
{
	FixedPoint a;
	a.m_nVal = m_nVal%longVal;
	return a;
}
#endif

FixedPoint FixedPoint::operator%(int intVal)
{
	FixedPoint a;
	a.m_nVal = m_nVal%intVal;
	return a;
}

FixedPoint FixedPoint::operator%(short shortVal)
{
	FixedPoint a;
	a.m_nVal = m_nVal%shortVal;
	return a;
}

bool FixedPoint::equals(FixedPoint b)
{
	return (m_nVal == b.m_nVal);
}

FixedPoint FixedPoint::add(FixedPoint b)
{
	FixedPoint a;
	a.m_nVal = m_nVal+b.m_nVal;
	return a;
}

FixedPoint FixedPoint::operator+(FixedPoint b)
{
	return add(b);
}

FixedPoint FixedPoint::subtract(FixedPoint b)
{
	FixedPoint a;
	a.m_nVal = m_nVal-b.m_nVal;
	return a;
}

FixedPoint FixedPoint::operator-(FixedPoint b)
{
	return subtract(b);
}

FixedPoint FixedPoint::multiply(FixedPoint b)
{
	FixedPoint a;
	a.m_nVal = (long)(((long long)m_nVal*b.m_nVal)/RESOLUTION);
	return a;
}

FixedPoint FixedPoint::operator*(FixedPoint b)
{
	return multiply(b);
}

FixedPoint FixedPoint::divide(FixedPoint b)
{
	if( b == 0 ) return FixedPoint(true, 0xFFFFFFFFL);
	FixedPoint a;
	if( m_nVal < 0xFFFFFFFFL/RESOLUTION )
		a.m_nVal = (m_nVal*RESOLUTION/b.m_nVal);
	else
		a.m_nVal = (long)(((long long)m_nVal*RESOLUTION/b.m_nVal));
	return a;
}

FixedPoint FixedPoint::operator/(FixedPoint b)
{
	return divide(b);
}

FixedPoint FixedPoint::add(float b)
{
	FixedPoint _b = b;
	return add(_b);
}

FixedPoint FixedPoint::operator+(float b)
{
	return add(b);
}

FixedPoint FixedPoint::subtract(float b)
{
	FixedPoint _b = b;
	return subtract(_b);
}

FixedPoint FixedPoint::operator-(float b)
{
	return subtract(b);
}

FixedPoint FixedPoint::multiply(float b)
{
	FixedPoint _b = b;
	return multiply(_b);
}

FixedPoint FixedPoint::operator*(float b)
{
	return multiply(b);
}

FixedPoint FixedPoint::divide(float b)
{
	FixedPoint _b = b;
	return divide(_b);
}

FixedPoint FixedPoint::operator/(float b)
{
	return divide(b);
}

#ifdef FIXED_HAS_DOUBLE
FixedPoint FixedPoint::add(double b)
{
	FixedPoint _b = b;
	return add(_b);
}

FixedPoint FixedPoint::operator+(double b)
{
	return add(b);
}

FixedPoint FixedPoint::subtract(double b)
{
	FixedPoint _b = b;
	return subtract(_b);
}

FixedPoint FixedPoint::operator-(double b)
{
	return subtract(b);
}

FixedPoint FixedPoint::multiply(double b)
{
	FixedPoint _b = b;
	return multiply(_b);
}

FixedPoint FixedPoint::operator*(double b)
{
	return multiply(b);
}

FixedPoint FixedPoint::divide(double b)
{
	FixedPoint _b = b;
	return divide(_b);
}

FixedPoint FixedPoint::operator/(double b)
{
	return divide(b);
}
#endif

FixedPoint FixedPoint::add(int b)
{
	FixedPoint _b = b;
	return add(_b);
}

FixedPoint FixedPoint::operator+(int b)
{
	return add(b);
}

FixedPoint FixedPoint::subtract(int b)
{
	FixedPoint _b = b;
	return subtract(_b);
}

FixedPoint FixedPoint::operator-(int b)
{
	return subtract(b);
}

FixedPoint FixedPoint::multiply(int b)
{
	FixedPoint _b = b;
	return multiply(_b);
}

FixedPoint FixedPoint::operator*(int b)
{
	return multiply(b);
}

FixedPoint FixedPoint::divide(int b)
{
	FixedPoint _b = b;
	return divide(_b);
}

FixedPoint FixedPoint::operator/(int b)
{
	return divide(b);
}

#ifdef FIXED_HAS_LONG
FixedPoint FixedPoint::add(long b)
{
	FixedPoint _b = b;
	return add(_b);
}

FixedPoint FixedPoint::operator+(long b)
{
	return add(b);
}

FixedPoint FixedPoint::subtract(long b)
{
	FixedPoint _b = b;
	return subtract(_b);
}

FixedPoint FixedPoint::operator-(long b)
{
	return subtract(b);
}

FixedPoint FixedPoint::multiply(long b)
{
	FixedPoint _b = b;
	return multiply(_b);
}

FixedPoint FixedPoint::operator*(long b)
{
	return multiply(b);
}

FixedPoint FixedPoint::divide(long b)
{
	FixedPoint _b = b;
	return divide(_b);
}

FixedPoint FixedPoint::operator/(long b)
{
	return divide(b);
}
#endif

FixedPoint FixedPoint::add(short b)
{
	FixedPoint _b = b;
	return add(_b);
}

FixedPoint FixedPoint::operator+(short b)
{
	return add(b);
}

FixedPoint FixedPoint::subtract(short b)
{
	FixedPoint _b = b;
	return subtract(_b);
}

FixedPoint FixedPoint::operator-(short b)
{
	return subtract(b);
}

FixedPoint FixedPoint::multiply(short b)
{
	FixedPoint _b = b;
	return multiply(_b);
}

FixedPoint FixedPoint::operator*(short b)
{
	return multiply(b);
}

FixedPoint FixedPoint::divide(short b)
{
	FixedPoint _b = b;
	return divide(_b);
}

FixedPoint FixedPoint::operator/(short b)
{
	return divide(b);
}

FixedPoint FixedPoint::operator*=(FixedPoint val)
{
	m_nVal = (long)(((long long)m_nVal*val.m_nVal)/RESOLUTION);
	return *this;
}

#ifdef FIXED_HAS_DOUBLE
FixedPoint FixedPoint::operator*=(double val)
{
	m_nVal = (long)(m_nVal*val);
	return *this;
}

FixedPoint FixedPoint::operator/=(double val)
{
	m_nVal = (long)(m_nVal/val);
	return *this;
}

FixedPoint FixedPoint::operator-=(double val)
{
	m_nVal -= (long)(val*RESOLUTION);
	return *this;
}

FixedPoint FixedPoint::operator+=(double val)
{
	m_nVal += (long)(val*RESOLUTION);
	return *this;
}

#endif
#ifdef FIXED_HAS_LONG
FixedPoint FixedPoint::operator*=(long val)
{
	m_nVal = (long)((long long)m_nVal*val);
	return *this;
}

FixedPoint FixedPoint::operator/=(long val)
{
	m_nVal = (long)(((long long)m_nVal*RESOLUTION)/val);
	return *this;
}

FixedPoint FixedPoint::operator-=(long val)
{
	m_nVal -= val*RESOLUTION;
	return *this;
}

FixedPoint FixedPoint::operator+=(long val)
{
	m_nVal += val*RESOLUTION;
	return *this;
}

#endif
FixedPoint FixedPoint::operator*=(float val)
{
	m_nVal = (long)(m_nVal*val);
	return *this;
}

FixedPoint FixedPoint::operator*=(int val)
{
	m_nVal = (long)((long long)m_nVal*val);
	return *this;
}

FixedPoint FixedPoint::operator*=(short val)
{
	m_nVal = (long)((long long)m_nVal*val);
	return *this;
}

FixedPoint FixedPoint::operator/=(FixedPoint val)
{
	m_nVal = (long)(((long long)m_nVal*RESOLUTION)/val.m_nVal);
	return *this;
}

FixedPoint FixedPoint::operator/=(float val)
{
	m_nVal = (long)(m_nVal/val);
	return *this;
}

FixedPoint FixedPoint::operator/=(int val)
{
	m_nVal = (long)(((long long)m_nVal*RESOLUTION)/val);
	return *this;
}

FixedPoint FixedPoint::operator/=(short val)
{
	m_nVal = (long)(((long long)m_nVal*RESOLUTION)/val);
	return *this;
}

FixedPoint FixedPoint::operator-=(FixedPoint val)
{
	m_nVal -= val.m_nVal;
	return *this;
}

FixedPoint FixedPoint::operator-=(float val)
{
	m_nVal -= (long)(val*RESOLUTION);
	return *this;
}

FixedPoint FixedPoint::operator-=(int val)
{
	m_nVal -= val*RESOLUTION;
	return *this;
}

FixedPoint FixedPoint::operator-=(short val)
{
	m_nVal -= val*RESOLUTION;
	return *this;
}

FixedPoint FixedPoint::operator+=(FixedPoint val)
{
	m_nVal += val.m_nVal;
	return *this;
}

FixedPoint FixedPoint::operator+=(float val)
{
	m_nVal += (long)(val*RESOLUTION);
	return *this;
}

FixedPoint FixedPoint::operator+=(int val)
{
	m_nVal += val*RESOLUTION;
	return *this;
}

FixedPoint FixedPoint::operator+=(short val)
{
	m_nVal += val*RESOLUTION;
	return *this;
}

#ifdef FIXED_HAS_DOUBLE
FixedPoint operator-(double a, FixedPoint b)
{
	FixedPoint _a = a;
	return _a - b;
}
#endif

FixedPoint operator-(float a, FixedPoint b)
{
	FixedPoint _a = a;
	return _a - b;
}

#ifdef FIXED_HAS_LONG
FixedPoint operator-(long a, FixedPoint b)
{
	FixedPoint _a = a;
	return _a - b;
}
#endif

FixedPoint operator-(int a, FixedPoint b)
{
	FixedPoint _a = a;
	return _a - b;
}

FixedPoint operator-(short a, FixedPoint b)
{
	FixedPoint _a = a;
	return _a - b;
}

#ifdef FIXED_HAS_LONG
FixedPoint operator*(FixedPoint a, long b)
{
	return a.multiply(b);
}
#endif

float operator+=(float& a, FixedPoint b)
{
	return a += (float)b;
}

#ifdef FIXED_HAS_DOUBLE
double operator+=(double& a, FixedPoint b)
{
	return a += (double)b;
}
#endif

float operator-=(float& a, FixedPoint b)
{
	return a -= (float)b;
}

#ifdef FIXED_HAS_DOUBLE
double operator-=(double& a, FixedPoint b)
{
	return a -= (double)b;
}

double operator*=(double& a, FixedPoint b)
{
	return a *= (double)b;
}

double operator/=(double& a, FixedPoint b)
{
	return a /= (double)b;
}

bool operator<(double b, FixedPoint a)
{
	return a >= b;
}

#endif

float operator*=(float& a, FixedPoint b)
{
	return a *= (float)b;
}

float operator/=(float& a, FixedPoint b)
{
	return a /= (float)b;
}

bool operator<(float b, FixedPoint a)
{
	return a >= b;
}

#ifdef FIXED_HAS_LONG
bool operator<(long b, FixedPoint a)
{
	return a >= b;
}
#endif

bool operator<(int b, FixedPoint a)
{
	return a >= b;
}

bool operator<(short b, FixedPoint a)
{
	return a >= b;
}

FixedPoint operator-(FixedPoint a)
{
	return 0-a;
}

/*
static
unsigned int isqrt(unsigned long n)
{
    unsigned long i;
    unsigned long k0, k1, nn;

    for (nn = i = n, k0 = 2; i > 0; i >>= 2, k0 <<= 1)
        ;
    nn <<= 2;
    for (;;) 
	{
        k1 = (nn / k0 + k0) >> 1;
        if (((k0 ^ k1) & ~1) == 0)
            break;
        k0 = k1;
    }
    return (unsigned int) ((k1 + 1) >> 1);
}
*/

static
FixedPoint isqrtB(FixedPoint p_Square)
{
	FixedPoint   res;
	FixedPoint   delta;
	FixedPoint   maxError;

	if( p_Square <= 0 )
		return 0;

	/* start at half */
	res = (p_Square / 2);

	/* determine allowable error */
	maxError =  (p_Square * sqrt_error);

	do
	{
		delta =  (( res * res ) - p_Square);
		res -=  (delta / ( res * 2 ));
	}
	while( delta > maxError || delta < -maxError );

	return res;
}

static
long iabs( register long p_Base )
{
	if( p_Base < 0 ) return -p_Base;
	return p_Base;
}

FixedPoint absx( FixedPoint p_Base )
{
	if( p_Base < 0 ) return -p_Base;
	return p_Base;
}
/*
                     (x^h) - 1
   ln(x)  =   lim    -------      
             h -> 0     h

*/

static
FixedPoint iLog2( FixedPoint p_Base )
{   
    FixedPoint w = 0;
	FixedPoint y = 0;
	FixedPoint z = 0;
	int num = 1;
	int dec = 0;

	if( p_Base == 1 )
		return 0;

	for( dec=0 ; absx( p_Base ) >= 2 ; ++dec )
		p_Base /= XLN_E;

	p_Base -= 1;
	z = p_Base;
	y = p_Base;
	w = 1;

	while( y != y + w )
		y += ( w = ( z = 0 - ( z * p_Base ) ) / ( num += 1 ) );

	return y + dec;
}

/*
	calculate the exponential function using the following series :

                          x^2     x^3     x^4     x^5
	exp(x) == 1  +  x  +  ---  +  ---  +  ---  +  ---  ...
                           2!      3!      4!      5!

*/

static
FixedPoint iExp2(FixedPoint p_Base)
{
	FixedPoint w;
	FixedPoint y;
	int num;

	for( w=1, y=1, num=1 ; y != y+w ; ++num )
		y += ( w *= p_Base / num );

	return y;
}

static
FixedPoint ipow( FixedPoint p_Base, FixedPoint p_Power )
{
	if( p_Base < 0 && p_Power%2 != 0 )
		return - iExp2( (p_Power * iLog2( -p_Base )) );
	else
		return iExp2( (p_Power * iLog2(absx( p_Base ))) );
}

static
FixedPoint ilog10( FixedPoint p_Base )
{
	return iLog2( p_Base ) / XLN_10;
}

FixedPoint FixedPoint::sqrt(void)
{
	//return FixedPoint(true, isqrt(m_nVal)*(RESOLUTION/1000L));
	return isqrtB(*this);
}

FixedPoint sqrtx(FixedPoint fixedVal)
{
	return fixedVal.sqrt();
}

FixedPoint FixedPoint::pow(FixedPoint fixedPower)
{
	return ipow(*this, fixedPower);
}

FixedPoint powx(FixedPoint fixedVal, FixedPoint fixedPower)
{
	return fixedVal.pow(fixedPower);
}

FixedPoint FixedPoint::exp(void)
{
	return iExp2(*this);
}

FixedPoint expx(FixedPoint fixedVal)
{
	return fixedVal.exp();
}

FixedPoint FixedPoint::log10(void)
{
	return ilog10(*this);
}

FixedPoint log10x(FixedPoint fixedVal)
{
	return fixedVal.log10();
}

FixedPoint FixedPoint::log(void)
{
	return iLog2(*this);
/*
	Calculate the POW function by calling EXP :

                  Y      A                 
                 X   =  e    where A = Y * log(X)
*/
}

FixedPoint logx(FixedPoint fixedVal)
{
	return fixedVal.log();
}

FixedPoint floorx(FixedPoint fixedVal)
{
	return fixedVal.floor();
}

FixedPoint ceilx(FixedPoint fixedVal)
{
	return fixedVal.ceil();
}

//
// Taylor Algorythm
// x - x^3/3! + x^5/5! - x^7/7! + x^9/9! ........    
//
// Note: Make xresult a float to get more precision
//
// Only accurate from -PI/2 to PI/2

static
FixedPoint _sinx(FixedPoint x)
{
	FixedPoint xpwr;
	long xftl;
	FixedPoint xresult;
	bool positive;

	xresult = 0;
	xpwr = x;
	xftl = 1;
	positive = true;

	// Note: 12! largest for long
	for(int i = 1; i < 7; i+=2)
	{
		if( positive )
			xresult += (xpwr/xftl);
		else
			xresult -= (xpwr/xftl);

		xpwr *= x;
		xpwr *= x;
		xftl *= i+1;
		xftl *= i+2;
		positive = !positive;
	}

	return xresult;
}

FixedPoint FixedPoint::sin(void)
{
	FixedPoint xresult;
	bool bBottom = false;
	static FixedPoint xPI = XPI;
	static FixedPoint x2PI = X2PI;
	static FixedPoint xPIO2 = XPIO2;

	FixedPoint x(true, m_nVal%_X2PI);
	if( x < 0 )
		x += x2PI;

	if( x > xPI )
	{
		bBottom = true;
		x -= xPI;
	}

	if( x <= xPIO2 )
		xresult = _sinx(x);
	else
		xresult = _sinx(xPIO2-(x-xPIO2));

	if( bBottom )
		return -xresult;

	return xresult;
}

FixedPoint sinx(FixedPoint x)
{
	return x.sin();
}

FixedPoint FixedPoint::cos(void)
{
	FixedPoint xresult;
	bool bBottom = false;
	static FixedPoint xPI = XPI;
	static FixedPoint x2PI = X2PI;
	static FixedPoint xPIO2 = XPIO2;

	FixedPoint x(true, (m_nVal+_XPIO2)%_X2PI);
	if( x < 0 )
		x += x2PI;

	if( x > xPI )
	{
		bBottom = true;
		x -= xPI;
	}

	if( x <= xPIO2 )
		xresult = _sinx(x);
	else
		xresult = _sinx(xPIO2-(x-xPIO2));

	if( bBottom )
		return -xresult;

	return xresult;
}

FixedPoint cosx(FixedPoint x)
{
	return x.cos();
}

FixedPoint FixedPoint::tan(void)
{
	return sin()/cos();
}

FixedPoint tanx(FixedPoint x)
{
	return x.tan();
}
