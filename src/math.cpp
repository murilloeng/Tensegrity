//std
#include <cmath>

//math
#include "inc/math.hpp"

float norm(const float* v)
{
	return sqrt(inner(v, v));
}
float inner(const float* a, const float* b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}
float cross(const float* a, const float* b, unsigned i)
{
	const unsigned j = (i + 1) % 3;
	const unsigned k = (i + 2) % 3;
	return a[j] * b[k] - a[k] * b[j];
}

float fn(float t, unsigned n)
{
	if(fabs(t) < 2 * M_PI)
	{
		return funt(t, n);
	}
	else
	{
		if(n % 2)
		{
			const unsigned p = n / 2;
			return (p % 2 ? -1 : +1) / pow(t, n) * (sin(t) - sint(t, p));
		}
		else
		{
			const unsigned p = n / 2;
			return (p % 2 ? -1 : +1) / pow(t, n) * (cos(t) - cost(t, p));
		}
	}
}
float funt(float t, unsigned n)
{
	//data
	int s = 1;
	unsigned k = 0;
	float v = 0, z = 1, dv;
	float p = std::tgamma(n + 1);
	//compute
	while(true)
	{
		dv = s * z / p;
		if(v + dv == v)
		{
			return v;
		}
		else
		{
			k++;
			v += dv;
			s *= -1;
			z *= t * t;
			p *= 2 * k + n;
			p *= 2 * k + n - 1;
		}
	}
}
float cost(float t, unsigned n)
{
	int s = 1;
	unsigned a = 1;
	float v = 0, p = 1;
	for(unsigned k = 0; k < n; k++)
	{
		v += s * p / a;
		s *= -1;
		p *= t * t;
		a *= 2 * k + 1;
		a *= 2 * k + 2;
	}
	return v;
}
float sint(float t, unsigned n)
{
	int s = 1;
	unsigned a = 1;
	float v = 0, p = t;
	for(unsigned k = 0; k < n; k++)
	{
		v += s * p / a;
		s *= -1;
		p *= t * t;
		a *= 2 * k + 2;
		a *= 2 * k + 3;
	}
	return v;
}

void cross(float* c, const float* a, const float* b)
{
	for(unsigned i = 0; i < 3; i++)
	{
		c[i] = cross(a, b, i);
	}
}
void rotate(float* vb, const float* xc, const float* tr)
{
	//data
	float c1[3], c2[3];
	const float tn = norm(tr);
	const float f1 = fn(tn, 1);
	const float f2 = fn(tn, 2);
	//setup
	for(unsigned i = 0; i < 3; i++) vb[i] -= xc[i];
	//rotate
	cross(c1, tr, vb);
	cross(c2, tr, c1);
	for(unsigned i = 0; i < 3; i++)
	{
		vb[i] += xc[i] + f1 * c1[i] + f2 * c2[i];
	}
}