//math
#include "Math/inc/linear/vector.hpp"

//tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Control_Dof.hpp"

//constructors
Control_Dof::Control_Dof(void)
{
	return;
}

//destructor
Control_Dof::~Control_Dof(void)
{
	return;
}

//type
strategy_type Control_Dof::type(void) const
{
	return strategy_type::control_dof;
}

//compute
double Control_Dof::load_predictor(const Solver* solver) const
{
	const uint32_t index = solver->watch_dof();
	const math::vector& dx = solver->dof_increment();
	const math::vector& dxt = solver->dof_predictor();
	return dx[index] / dxt[index];
}
double Control_Dof::load_corrector(const Solver* solver) const
{
	const int32_t index = solver->watch_dof();
	const math::vector& ddxr = solver->dof_corrector(0);
	const math::vector& ddxt = solver->dof_corrector(1);
	return -ddxr[index] / ddxt[index];
}