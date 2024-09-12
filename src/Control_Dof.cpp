//tensegrity
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
	return 0;
}
double Control_Dof::load_corrector(const Solver* solver) const
{
	return 0;
}