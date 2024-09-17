set grid

na = 10
dp = 28.0
set ylabel "F (kN)"
set xlabel "u_3 (cm)"
set cblabel "{/Symbol q} (rad)"
set palette rgbformulae 33, 13, 10
set cbrange [0 : pi / nc]

plot for [i = 0 : na] sprintf("model-%d-full.txt", i) using (-100 * $3) : ($8) with lines linecolor palette frac real(i) / na notitle

set terminal pdf
set output 'angular.pdf'
replot