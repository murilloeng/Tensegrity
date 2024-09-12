#pragma once

//tensegrity
#include "Tensegrity/inc/Strategy.hpp"

class Arc_Length : public Strategy
{
public:
	//constructors
	Arc_Length(void);

	//destructor
	~Arc_Length(void);

	//type
	strategy_type type(void) const override;

	//compute
	double load_predictor(const Solver*) const override;
	double load_corrector(const Solver*) const override;
};