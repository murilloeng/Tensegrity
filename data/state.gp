labels = "u_1 u_2 u_3 t_1 t_2 t_3"

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
set terminal pdf
set for [i = 1 : nc] arrow from 0, 0 to R * cos(2 * pi * i / nc + pi / nc), R * sin(2 * pi * i / nc + pi / nc) front nohead dashtype 2

do for [i = 1 : 6] {
	set title word(labels, i)
	set output sprintf('state_%d.pdf', i)
	splot 'state.txt' using ($1 * cos($2)) : ($1 * sin($2)) : (column(i + 2)) notitle
}