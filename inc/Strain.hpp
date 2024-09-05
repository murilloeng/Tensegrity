#pragma once

//std
#include <cstdint>

class Strain
{
public:
	enum class strain_measure : uint32_t
	{
		quadratic,
		logarithmic
	};

	static double strain(double, strain_measure);
	static double strain_hessian(double, strain_measure);
	static double strain_gradient(double, strain_measure);

private:
	static double quadratic_strain(double);
	static double quadratic_strain_hessian(double);
	static double quadratic_strain_gradient(double);

	static double logarithmic_strain(double);
	static double logarithmic_strain_hessian(double);
	static double logarithmic_strain_gradient(double);
};