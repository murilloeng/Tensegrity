#pragma once

//std
#include <cstdint>

enum class strategy_type : uint32_t
{
	arc_length,
	control_dof,
	control_load
};

class Solver;

class Strategy
{
public:
	//constructors
	Strategy(void);

	//destructor
	virtual ~Strategy(void);

	//type
	virtual strategy_type type(void) const = 0;

	//create
	static Strategy* create(Strategy*&, strategy_type);

	//compute
	virtual double load_predictor(const Solver*) const = 0;
	virtual double load_corrector(const Solver*) const = 0;
};