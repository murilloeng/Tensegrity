#pragma once

//std
#include <cstdint>

void test_force(void);
void test_stiffness(void);

void fun_force(double*, const double*, void**);
void fun_energy(double*, const double*, void**);
void fun_stiffness(double*, const double*, void**);

void load_nonlinear_vertical_radial(uint32_t);
void load_nonlinear_vertical_angular(uint32_t);
void load_nonlinear_vertical_radial_peak(uint32_t);
void load_nonlinear_vertical_angular_peak(uint32_t);