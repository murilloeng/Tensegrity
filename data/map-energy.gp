set pm3d
set grid
set view map
set key above
unset surface
set xlabel "x_1"
set ylabel "x_2"
set size ratio 1
set format x '%.2f'
set format y '%.2f'
set format cb '%.2e'
set palette rgbformulae 33, 13, 10

R = 0.14
dt = pi / nc
a(i) = 2 * dt * i

set terminal pdf
set label "" at 0, 0 front point pointtype 6
set for [i = 1 : nc] label "" at R * cos(a(i)), R * sin(a(i)) front point pointtype 6
set for [i = 1 : nc] arrow from 0, 0 to R * cos(a(i) + dt), R * sin(a(i) + dt) front nohead dashtype 2

set output 'energy.pdf'
set title 'Internal Energy'
splot 'energy.txt' using ($1 * cos($2)) : ($1 * sin($2)) : ($3) notitle