//tensegrity
#include "Tensegrity/inc/Arc_Length.hpp"

//constructors
Arc_Length::Arc_Length(void)
{
	return;
}

//destructor
Arc_Length::~Arc_Length(void)
{
	return;
}

//type
strategy_type Arc_Length::type(void) const
{
	return strategy_type::arc_length;
}

//compute
double Arc_Length::load_predictor(const Solver* solver) const
{
	return 0;
}
double Arc_Length::load_corrector(const Solver* solver) const
{
	return 0;
}