#data
labels = "u_1 u_2 u_3 t_1 t_2 t_3"

#setup
set grid
set key above
set ylabel "U"
set size ratio 1
set format x '%.2f'
set format y '%.2e'
set xlabel word(labels, index)

#plot
plot 'data/energy.txt' using (column(2 * index - 1)) : (column(2 * index + 0)) with lines linecolor rgb "#0000ff" notitle