set pm3d
set grid
# set view map
set key above
# unset surface
# set encoding utf8
# set isosamples 200
set format x '%.2f'
set format y '%.2f'
# set cbrange [0 : 1]
# set samples 200, 200
set palette rgbformulae 33, 13, 10

splot 'load_vertical.txt' using ($1 * cos($2)) : ($1 * sin($2)) : ($8)