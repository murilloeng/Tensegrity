#pragma once

//std
#include <cstdint>

void test_force(void);
void test_stiffness(void);

void map(uint32_t, uint32_t, double);

void fun_force(double*, const double*, void**);
void fun_energy(double*, const double*, void**);
void fun_stiffness(double*, const double*, void**);

void load_nonlinear_vertical_radial(uint32_t);
void load_nonlinear_vertical_angular(uint32_t);
void load_nonlinear_vertical_radial_peak(uint32_t);
void load_nonlinear_vertical_angular_peak(uint32_t);

void load_nonlinear_horizontal_radial(uint32_t, bool);
void load_nonlinear_horizontal_angular(uint32_t, bool);
void load_nonlinear_horizontal_radial_peak(uint32_t, bool);
void load_nonlinear_horizontal_angular_peak(uint32_t, bool);