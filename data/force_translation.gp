set grid
set key above
set xlabel "u"
set ylabel "F/M"
set terminal windows

plot "data/force_translation.txt" using 1:6 with lines linecolor rgb "#0000ff" notitle