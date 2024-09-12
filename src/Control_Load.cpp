//tensegrity
#include "Tensegrity/inc/Solver.hpp"
#include "Tensegrity/inc/Control_Load.hpp"

//constructors
Control_Load::Control_Load(void)
{
	return;
}

//destructor
Control_Load::~Control_Load(void)
{
	return;
}

//type
strategy_type Control_Load::type(void) const
{
	return strategy_type::control_load;
}

//compute
double Control_Load::load_predictor(const Solver* solver) const
{
	return solver->load_predictor();
}
double Control_Load::load_corrector(const Solver* solver) const
{
	return 0;
}