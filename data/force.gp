# #data
i1 = 7 * (index_1 - 1) + 1
i2 = 7 * (index_1 - 1) + 1 + index_2
labels_2 = "F_1 F_2 F_3 M_1 M_2 M_3"
labels_1 = "u_1 u_2 u_3 t_1 t_2 t_3"

#setup
set grid
set key above
set format x '%.2f'
set format y '%.2e'
set xlabel word(labels_1, index_1)
set ylabel word(labels_2, index_2) norotate

#plot
plot 'data/force.txt' using (column(i1)) : (column(i2)) with lines linecolor rgb "#0000ff" notitle

#range
min(a, b) = a < b ? a : b
max(a, b) = a > b ? a : b
set yrange [min(GPVAL_Y_MIN, -1) : max(GPVAL_Y_MAX, +1)]
replot