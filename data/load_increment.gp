labels = "u_1 u_2 u_3 t_1 t_2 t_3"

set pm3d
set grid
set view map
set key above
unset surface
set xlabel "x_1"
set ylabel "x_2"
set size ratio 1
set terminal png
set format x '%.2f'
set format y '%.2f'
set format cb '%.2e'
set title word(labels, index_1)
set palette rgbformulae 33, 13, 10
set cbrange [-8.00e-04 : 8.00e-04]
#set cbrange [-6.00e-04 : -3.00e-04] # Ht6 -> U3

do for [i = 0 : 100] {
    set output sprintf('data/load_increment_Hc6_%02d.png', i)
    splot sprintf('data/load_increment_Hc6_%02d.txt', i) using ($1 * cos($2)) : ($1 * sin($2)) : (column(index_1 + 2)) notitle
}