//math
#include "Math/inc/linear/vector.hpp"

//tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Control_Dof.hpp"

//constructors
Control_Dof::Control_Dof(void) : m_dof_index(0)
{
	return;
}

//destructor
Control_Dof::~Control_Dof(void)
{
	return;
}

//data
uint32_t Control_Dof::dof_index(void) const
{
	return m_dof_index;
}
uint32_t Control_Dof::dof_index(uint32_t dof_index)
{
	return m_dof_index = dof_index;
}

//type
strategy_type Control_Dof::type(void) const
{
	return strategy_type::control_dof;
}

//compute
double Control_Dof::load_predictor(const Solver* solver) const
{
	const math::vector& dx = solver->dof_increment();
	const math::vector& dxt = solver->dof_predictor();
	return dx[m_dof_index] / dxt[m_dof_index];
}
double Control_Dof::load_corrector(const Solver* solver) const
{
	const math::vector& ddxr = solver->dof_corrector(0);
	const math::vector& ddxt = solver->dof_corrector(1);
	return -ddxr[m_dof_index] / ddxt[m_dof_index];
}