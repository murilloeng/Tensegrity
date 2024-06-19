#data
labels_2 = "F_1 F_2 F_3 M_1 M_2 M_3"
labels_1 = "u_1 u_2 u_3 t_1 t_2 t_3"

#setup
set grid
set key above
set size ratio 1
set format x '%.2f'
set format y '%.2e'
set xlabel word(labels_1, index_1)
set ylabel word(labels_2, index_2)

#plot
plot 'data/force.txt' using ($1) : (column(index_2 + 1)) with lines linecolor rgb "#0000ff" notitle