#pragma once

//std
#include <cstdint>

//tensegrity
#include "Tensegrity/inc/Strategy.hpp"

class Control_Dof : public Strategy
{
public:
	//constructors
	Control_Dof(void);

	//destructor
	~Control_Dof(void);

	//type
	strategy_type type(void) const override;

	//compute
	double load_predictor(const Solver*) const override;
	double load_corrector(const Solver*) const override;
};