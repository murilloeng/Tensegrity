#data
na = 10
labels = "u_1 u_2 u_3"
data_file(i) = sprintf("%d/model-%d-state-full.txt", nc, i)

#setup
set grid
set terminal pdf
set ylabel "F (kN)"
set cbrange [0 : pi / nc]
set cblabel "{/Symbol q} (rad)"
set palette rgbformulae 33, 13, 10
set xlabel sprintf("%s (cm)", word(labels, dof_index))
set output sprintf('%d/vertical-angular-%d-%d.pdf', nc, nc, dof_index)

#plot
plot for [i = 0 : na] data_file(i) using (100 * column(dof_index)) : ($8) with lines linecolor palette frac real(i) / na notitle