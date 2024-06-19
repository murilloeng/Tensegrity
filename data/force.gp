# #data
i = 7 * (index - 1) + 1
xl = index < 4 ? 0.1 : pi / 3
labels_2 = "F_1 F_2 F_3 M_1 M_2 M_3"
labels_1 = "u_1 u_2 u_3 t_1 t_2 t_3"

#setup
set grid
set key above
set format x '%.2f'
set format y '%.2e'
set xtics -xl, xl, xl

#plot
set multiplot layout 3, 2 columnsfirst title word(labels_1, index)
set ylabel word(labels_2, 1) norotate
plot 'data/force.txt' using (column(i)) : (column(i + 1)) with lines linecolor rgb "#0000ff" notitle
set ylabel word(labels_2, 2) norotate
plot 'data/force.txt' using (column(i)) : (column(i + 2)) with lines linecolor rgb "#0000ff" notitle
set ylabel word(labels_2, 3) norotate
plot 'data/force.txt' using (column(i)) : (column(i + 3)) with lines linecolor rgb "#0000ff" notitle
set ylabel word(labels_2, 4) norotate
plot 'data/force.txt' using (column(i)) : (column(i + 4)) with lines linecolor rgb "#0000ff" notitle
set ylabel word(labels_2, 5) norotate
plot 'data/force.txt' using (column(i)) : (column(i + 5)) with lines linecolor rgb "#0000ff" notitle
set ylabel word(labels_2, 6) norotate
plot 'data/force.txt' using (column(i)) : (column(i + 6)) with lines linecolor rgb "#0000ff" notitle
unset multiplot