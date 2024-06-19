set grid
set key above
set ylabel "U"
set size ratio 1
set format x '%.2f'
set format y '%.2e'

plot 'energy.txt' using ($1) : ($2) with lines linecolor rgb "#0000ff" notitle