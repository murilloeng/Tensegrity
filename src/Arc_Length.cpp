//std
#include <cmath>

//math
#include "Math/inc/misc/misc.hpp"
#include "Math/inc/linear/vector.hpp"

//tensegrity
#include "Tensegrity/inc/Solver.hpp"
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
	//data
	const math::vector& dx = solver->dof_increment();
	const math::vector& dxt = solver->dof_predictor();
	//return
	const double d = dx.norm();
	const double t = dxt.norm();
	const double s = dx.inner(dxt);
	return math::sign(s) * d / t;
}
double Arc_Length::load_corrector(const Solver* solver) const
{
	//data
	const math::vector& dx = solver->dof_increment();
	const math::vector& ddxr = solver->dof_corrector(0);
	const math::vector& ddxt = solver->dof_corrector(1);
	//return
	const double s = dx.inner(ddxt);
	const double a = ddxt.inner(ddxt);
	const double b = ddxt.inner(ddxr + dx);
	const double c = ddxr.inner(ddxr + 2 * dx);
	return -b / a + math::sign(s) * sqrt(pow(b / a, 2) - c / a);
}