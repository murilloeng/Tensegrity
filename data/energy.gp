#data
i1 = 2 * index_1 - 1
i2 = 2 * index_1 + 0
labels = "u_1 u_2 u_3 t_1 t_2 t_3"

#setup
set grid
set key above
set format x '%.2f'
set format y '%.2e'
set ylabel "U" norotate
set xlabel word(labels, index_1)

#plot
plot 'data/energy.txt' using (column(i1)) : (column(i2)) with lines linecolor rgb "#0000ff" notitle