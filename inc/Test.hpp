#pragma once

//std
#include <cstdint>

//tensegrity
#include "Tensegrity/inc/Tensegrity.hpp"

class Test
{
public:
	//constructors
	Test(void);

	//destructor
	~Test(void);

	//data
	Tensegrity& base(void);

	double force(double);
	double force(void) const;

	double tension(double);
	double tension(void) const;

	uint32_t cables(uint32_t);
	uint32_t cables(void) const;

	uint16_t mesh_angle(uint32_t);
	uint16_t mesh_angle(void) const;

	uint16_t mesh_radius(uint32_t);
	uint16_t mesh_radius(void) const;

	//solve
	void solve(void);

private:
	//setup
	void setup(void);
	void write_state(void) const;
	void write_cables(void) const;
	void write_energy(void) const;

	//path
	void path(char*) const;

	//data
	double m_force;

	Tensegrity m_base;

	uint32_t m_mesh_angle;
	uint32_t m_mesh_radius;

	double* m_data_state;
	double* m_data_cables;
	double* m_data_energy;
};
