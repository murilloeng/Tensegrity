#data
nr = 10
dp = 28.0
labels = "u_1 u_2 u_3"
data_file(i) = sprintf("%d/model-%d-state-full.txt", nc, i - 1)

#setup
set grid
set terminal pdf
set ylabel "F (kN)"
set cblabel "r (cm)"
set palette rgbformulae 33, 13, 10
set cbrange [dp / 2 / nr : dp / 2]
set xlabel sprintf("%s (cm)", word(labels, dof_index))
set output sprintf('%d/horizontal-1-radial-%d-%d.pdf', nc, nc, dof_index)

#plot
plot for [i = 1 : nr] data_file(i) using (100 * column(dof_index)) : ($8) with lines linecolor palette frac real(i - 1) / (nr - 1) notitle