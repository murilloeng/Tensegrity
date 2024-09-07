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
set title word(labels, index)
set palette rgbformulae 33, 13, 10

Rr = 0.14
splot 'data/force.txt' using ($1 * cos($2)) : ($1 * sin($2)) : (column(index + 2)) notitle

replot