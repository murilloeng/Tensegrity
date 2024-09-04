#pragma once

//std
#include <functional>

//math
#include "Math/inc/linear/vec3.hpp"
#include "Math/inc/linear/quat.hpp"
#include "Math/inc/linear/mat3.hpp"

class Solver;

class Tensegrity
{
public:
	//constructors
	Tensegrity(void);

	//destructor
	~Tensegrity(void);

	//data
	uint32_t cables(uint32_t);
	uint32_t cables(void) const;

	double rod_length(double);
	double rod_length(void) const;

	double twist_angle(double);
	double twist_angle(void) const;

	double height_total(double);
	double height_total(void) const;

	double height_center(double);
	double height_center(void) const;

	double plate_diameter(double);
	double plate_diameter(void) const;

	double plate_thickness(double);
	double plate_thickness(void) const;

	double cables_diameter(double);
	double cables_diameter(void) const;

	const double* yield_stress(double);
	const double* yield_stress(void) const;
	const double* yield_stress(const double*);

	const double* residual_stress(double);
	const double* residual_stress(void) const;
	const double* residual_stress(const double*);

	const double* elastic_modulus(double);
	const double* elastic_modulus(void) const;
	const double* elastic_modulus(const double*);

	//formulation
	double internal_energy(void) const;
	double potential_energy(void) const;
	
	void stiffness(math::matrix&) const;
	
	void internal_force(math::vector&) const;
	void external_force(math::vector&) const;

	//friends
	friend class Solver;

private:
	//position
	math::vec3 position(unsigned, bool, bool) const;

	//data
	double* m_Ec;
	double* m_sr;
	double* m_sy;
	unsigned m_nc;
	bool m_inelastic;
	char m_label[200];
	double m_rod_length;
	double m_twist_angle;
	double m_height_total;
	double m_height_center;
	double m_plate_diameter;
	double m_plate_thickness;
	double m_cables_diameter;

	Solver* m_solver;

	std::vector<math::vec3> m_ak;
	std::vector<math::vec3> m_pk;
};