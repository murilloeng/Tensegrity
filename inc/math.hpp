#pragma once

//math
float norm(const float*);
float inner(const float*, const float*);
float cross(const float*, const float*, unsigned);

float fn(float, unsigned);
float funt(float, unsigned);
float cost(float, unsigned);
float sint(float, unsigned);

void cross(float*, const float*, const float*);
void rotate(float*, const float*, const float*);