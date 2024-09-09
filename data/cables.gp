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
	set terminal windows
	set title sprintf("Cable %d", i)
	splot 'cables.txt' using ($1 * cos($2)) : ($1 * sin($2)) : (column(i + 2)) notitle
	R = 0.14
	x = i == 1 ? 0 : R * cos(2 * pi * (i - 1) / nc)
	y = i == 1 ? 0 : R * sin(2 * pi * (i - 1) / nc)
	set terminal pdf
	set output sprintf('cables_%d.pdf', i - 1)
	set label 1 "" at x, y point pointtype 7 linecolor rgb "#000000" front
	replot
	unset label 1
}