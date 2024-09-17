set grid
set key above

nr = 10
dp = 28.0
set ylabel "F (kN)"
set cbrange [3 : 10]
set xlabel "{/Symbol q} (rad)"
set palette rgbformulae 33, 13, 10

plot \
	for [i = 3 : 10] sprintf("%d/peak.txt", i) using ($1) : ($2) with lines linecolor palette cb i notitle,\
	for [i = 3 : 10] sprintf("%d/peak.txt", i) using ($1) : ($2) every 10 with points linecolor palette cb i pointtype i - 2 title sprintf("n_c = %d", i)

set terminal pdf
set output 'peak.pdf'
replot