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

do for [i = 1 : nc + 1] {
	set terminal pdf
	set title sprintf("Cable %d", i)
	set output sprintf('cables_%d.pdf', i - 1)
	splot 'cables.txt' using ($1 * cos($2)) : ($1 * sin($2)) : (column(i + 2)) notitle
}