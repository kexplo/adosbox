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

#ifndef _FIXED_H
#define _FIXED_H

 // Allow floating point input
#define FIXED_HAS_DOUBLE

 // Allow longs
#define FIXED_HAS_LONG

class FixedPoint
{
private:
	long	m_nVal;
public:
	FixedPoint(void);
	FixedPoint(const FixedPoint& fixedVal);
	FixedPoint(const FixedPoint* fixedVal);
	FixedPoint(bool bInternal, long nVal);
	FixedPoint(long nVal);
	FixedPoint(long long nVal);
	FixedPoint(int nVal);
	FixedPoint(short nVal);
#ifdef FIXED_HAS_DOUBLE
	FixedPoint(double nVal);
#endif
	FixedPoint(float nVal);
	~FixedPoint(void);
	FixedPoint operator++(void);
	FixedPoint operator--(void);
	FixedPoint& operator=(float floatVal);
#ifdef FIXED_HAS_DOUBLE
	FixedPoint& operator=(double floatVal);
#endif
	FixedPoint& operator=(FixedPoint fixedVal);
	FixedPoint& operator=(int intVal);
	FixedPoint& operator=(unsigned int intVal);
#ifdef FIXED_HAS_LONG
	FixedPoint& operator=(long longVal);
	FixedPoint& operator=(unsigned long longVal);
#endif
	FixedPoint& operator=(short shortVal);
	FixedPoint& operator=(unsigned short shortVal);
	FixedPoint& operator=(char charVal);
	FixedPoint& operator=(unsigned char charVal);
	bool equals(FixedPoint b);
	bool operator==(float floatVal);
#ifdef FIXED_HAS_DOUBLE
	bool operator==(double floatVal);
#endif
	bool operator==(FixedPoint fixedVal);
	bool operator==(int intVal);
#ifdef FIXED_HAS_LONG
	bool operator==(long intVal);
#endif
	bool operator==(short intVal);
	bool operator!=(float floatVal);
#ifdef FIXED_HAS_DOUBLE
	bool operator!=(double floatVal);
#endif
	bool operator!=(FixedPoint fixedVal);
	bool operator!=(int intVal);
#ifdef FIXED_HAS_LONG
	bool operator!=(long intVal);
#endif
	bool operator!=(short intVal);
	bool lessthan(FixedPoint b);
	bool operator<(float floatVal);
#ifdef FIXED_HAS_DOUBLE
	bool operator<(double floatVal);
#endif
	bool operator<(FixedPoint fixedVal);
	bool operator<(int intVal);
#ifdef FIXED_HAS_LONG
	bool operator<(long intVal);
#endif
	bool operator<(short intVal);
	bool lessthanequal(FixedPoint b);
	bool operator<=(float floatVal);
#ifdef FIXED_HAS_DOUBLE
	bool operator<=(double floatVal);
#endif
	bool operator<=(FixedPoint fixedVal);
	bool operator<=(int intVal);
#ifdef FIXED_HAS_LONG
	bool operator<=(long intVal);
#endif
	bool operator<=(short intVal);
	bool operator>(float floatVal);
#ifdef FIXED_HAS_DOUBLE
	bool operator>(double floatVal);
#endif
	bool operator>(FixedPoint fixedVal);
	bool operator>(int intVal);
#ifdef FIXED_HAS_LONG
	bool operator>(long intVal);
#endif
	bool operator>(short intVal);
	bool operator>=(float floatVal);
#ifdef FIXED_HAS_DOUBLE
	bool operator>=(double floatVal);
#endif
	bool operator>=(FixedPoint fixedVal);
	bool operator>=(int intVal);
#ifdef FIXED_HAS_LONG
	bool operator>=(long intVal);
#endif
	bool operator>=(short intVal);
	operator float(void);
#ifdef FIXED_HAS_DOUBLE
	operator double(void);
#endif
	operator unsigned short(void);
#ifdef FIXED_HAS_LONG
	long GetLong(void);
	operator long(void);
#endif
	operator short(void);
	operator int(void);
	FixedPoint floor(void);
	FixedPoint ceil(void);
	FixedPoint add(FixedPoint b);
	FixedPoint subtract(FixedPoint b);
	FixedPoint multiply(FixedPoint b);
	FixedPoint divide(FixedPoint b);
	FixedPoint operator+(FixedPoint b);
	FixedPoint operator-(FixedPoint b);
	FixedPoint operator*(FixedPoint b);
	FixedPoint operator/(FixedPoint b);
	FixedPoint add(float b);
	FixedPoint subtract(float b);
	FixedPoint multiply(float b);
	FixedPoint divide(float b);
	FixedPoint operator+(float b);
	FixedPoint operator-(float b);
	FixedPoint operator*(float b);
	FixedPoint operator/(float b);
#ifdef FIXED_HAS_DOUBLE
	FixedPoint add(double b);
	FixedPoint subtract(double b);
	FixedPoint multiply(double b);
	FixedPoint divide(double b);
	FixedPoint operator+(double b);
	FixedPoint operator-(double b);
	FixedPoint operator*(double b);
	FixedPoint operator/(double b);
#endif
	FixedPoint add(int b);
	FixedPoint subtract(int b);
	FixedPoint multiply(int b);
	FixedPoint divide(int b);
	FixedPoint operator+(int b);
	FixedPoint operator-(int b);
	FixedPoint operator*(int b);
	FixedPoint operator/(int b);
#ifdef FIXED_HAS_LONG
	FixedPoint add(long b);
	FixedPoint subtract(long b);
	FixedPoint multiply(long b);
	FixedPoint divide(long b);
	FixedPoint operator+(long b);
	FixedPoint operator-(long b);
	FixedPoint operator*(long b);
	FixedPoint operator/(long b);
#endif
	FixedPoint add(short b);
	FixedPoint subtract(short b);
	FixedPoint multiply(short b);
	FixedPoint divide(short b);
	FixedPoint operator+(short b);
	FixedPoint operator-(short b);
	FixedPoint operator*(short b);
	FixedPoint operator/(short b);
	FixedPoint sqrt(void);
	FixedPoint pow(FixedPoint fixedPower);
	FixedPoint log10(void);
	FixedPoint log(void);
	FixedPoint exp(void);
	FixedPoint cos(void);
	FixedPoint sin(void);
	FixedPoint tan(void);
	FixedPoint operator%(FixedPoint fixedVal);
#ifdef FIXED_HAS_LONG
	FixedPoint operator%(long longVal);
#endif
	FixedPoint operator%(int intVal);
	FixedPoint operator%(short shortVal);
	FixedPoint operator*=(FixedPoint val);
#ifdef FIXED_HAS_DOUBLE
	FixedPoint operator*=(double val);
	FixedPoint operator/=(double val);
	FixedPoint operator-=(double val);
	FixedPoint operator+=(double val);
#endif
#ifdef FIXED_HAS_LONG
	FixedPoint operator*=(long val);
	FixedPoint operator/=(long val);
	FixedPoint operator-=(long val);
	FixedPoint operator+=(long val);
#endif
	FixedPoint operator*=(float val);
	FixedPoint operator*=(int val);
	FixedPoint operator*=(short val);
	FixedPoint operator/=(FixedPoint val);
	FixedPoint operator/=(float val);
	FixedPoint operator/=(int val);
	FixedPoint operator/=(short val);
	FixedPoint operator-=(FixedPoint val);
	FixedPoint operator-=(float val);
	FixedPoint operator-=(int val);
	FixedPoint operator-=(short val);
	FixedPoint operator+=(FixedPoint val);
	FixedPoint operator+=(float val);
	FixedPoint operator+=(int val);
	FixedPoint operator+=(short val);
};

#ifdef FIXED_HAS_DOUBLE
FixedPoint operator-(double a, FixedPoint b);
#endif
FixedPoint operator-(float a, FixedPoint b);
#ifdef FIXED_HAS_LONG
FixedPoint operator-(long a, FixedPoint b);
#endif
FixedPoint operator-(int a, FixedPoint b);
FixedPoint operator-(short a, FixedPoint b);

#ifdef FIXED_HAS_DOUBLE
double operator+=(double& a, FixedPoint b);
double operator-=(double& a, FixedPoint b);
double operator*=(double& a, FixedPoint b);
double operator/=(double& a, FixedPoint b);
#endif
float operator+=(float& a, FixedPoint b);
float operator-=(float& a, FixedPoint b);
float operator*=(float& a, FixedPoint b);
float operator/=(float& a, FixedPoint b);

#ifdef FIXED_HAS_DOUBLE
bool operator<(double b, FixedPoint a);
#endif
bool operator<(float b, FixedPoint a);
#ifdef FIXED_HAS_LONG
bool operator<(long b, FixedPoint a);
#endif
bool operator<(short b, FixedPoint a);
bool operator<(int b, FixedPoint a);

FixedPoint operator-(FixedPoint a);

FixedPoint absx( FixedPoint p_Base );
FixedPoint floorx(FixedPoint fixedVal);
FixedPoint ceilx(FixedPoint fixedVal);
FixedPoint sqrtx(FixedPoint fixedVal);
FixedPoint powx(FixedPoint fixedVal, FixedPoint fixedPower);
FixedPoint log10x(FixedPoint fixedVal);
FixedPoint logx(FixedPoint fixedVal);
FixedPoint expx(FixedPoint fixedVal);
FixedPoint sinx(FixedPoint x);
FixedPoint cosx(FixedPoint x);
FixedPoint tanx(FixedPoint x);

#endif // _FIXED_H

