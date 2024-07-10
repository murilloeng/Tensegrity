labels = "u_1 u_2 u_3 t_1 t_2 t_3"

labels_2 = "u1 u2 u3 t1 t2 t3"

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
set cbrange [-4.00e-04 : 4.00e-04]

do for [i = 0 : 100] {
    print(i)
    set output sprintf("data/load_3_Ht_3_%s_%02d.png", word(labels_2, index_1), i)
    splot sprintf("data/load_3_Ht_3_%02d.txt", i) using ($1 * cos($2)) : ($1 * sin($2)) : (column(index_1 + 2)) notitle
}