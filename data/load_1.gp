labels = "u_1 u_2 u_3 t_1 t_2 t_3"

set grid
set key above
set ylabel 'P'
set size ratio 1
set format x '%.2f'
set format y '%.2f'
set xlabel word(labels, index_1)

plot 'data/load_1.txt' using ($1) : (column(index_1 + 1)) with lines linecolor rgb "#0000ff" notitle