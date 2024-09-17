set grid

nr = 10
dp = 28.0
set ylabel "F (kN)"
set cblabel "r (cm)"
set xlabel "u_3 (cm)"
set palette rgbformulae 33, 13, 10
set cbrange [dp / 2 / nr : dp / 2]

plot for [i = 0 : nr - 1] sprintf("model-%d-full.txt", i) using (-100 * $3) : ($8) with lines linecolor palette frac real(i) / (nr - 1) notitle

set terminal pdf
set output 'radial.pdf'
replot