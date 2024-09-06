//std
#include <cmath>

//tensegrity
#include "Tensegrity/inc/strain.hpp"

double Strain::strain(double s, strain_measure measure)
{
	if(measure == strain_measure::quadratic)
	{
		return quadratic_strain(s);
	}
	if(measure == strain_measure::logarithmic)
	{
		return logarithmic_strain(s);
	}
	return 0;
}
double Strain::strain_hessian(double s, strain_measure measure)
{
	if(measure == strain_measure::quadratic)
	{
		return quadratic_strain_hessian(s);
	}
	if(measure == strain_measure::logarithmic)
	{
		return logarithmic_strain_hessian(s);
	}
	return 0;
}
double Strain::strain_gradient(double s, strain_measure measure)
{
	if(measure == strain_measure::quadratic)
	{
		return quadratic_strain_gradient(s);
	}
	if(measure == strain_measure::logarithmic)
	{
		return logarithmic_strain_gradient(s);
	}
	return 0;
}

double Strain::quadratic_strain(double s)
{
	return (s * s - 1) / 2;
}
double Strain::quadratic_strain_hessian(double s)
{
	return 1;
}
double Strain::quadratic_strain_gradient(double s)
{
	return s;
}

double Strain::logarithmic_strain(double s)
{
	return log(s);
}
double Strain::logarithmic_strain_hessian(double s)
{
	return -1 / s / s;
}
double Strain::logarithmic_strain_gradient(double s)
{
	return 1 / s;
}