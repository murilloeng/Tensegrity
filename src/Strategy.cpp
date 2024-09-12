//tensegrity
#include "Tensegrity/inc/Strategy.hpp"
#include "Tensegrity/inc/Arc_Length.hpp"
#include "Tensegrity/inc/Control_Dof.hpp"
#include "Tensegrity/inc/Control_Load.hpp"

//constructors
Strategy::Strategy(void)
{
	return;
}

//destructor
Strategy::~Strategy(void)
{
	return;
}

//create
Strategy* Strategy::create(Strategy*& strategy, strategy_type type)
{
	delete strategy;
	if(type == strategy_type::arc_length) strategy = new Arc_Length;
	if(type == strategy_type::control_dof) strategy = new Control_Dof;
	if(type == strategy_type::control_load) strategy = new Control_Load;
	return strategy;
}