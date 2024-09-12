#pragma once

//tensegrity
#include "Tensegrity/inc/Strategy.hpp"

class Control_Load : public Strategy
{
public:
	//constructors
	Control_Load(void);

	//destructor
	~Control_Load(void);

	//type
	strategy_type type(void) const override;

	//compute
	double load_predictor(const Solver*) const override;
	double load_corrector(const Solver*) const override;
};