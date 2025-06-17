@echo off

echo 0-3-0
cd 0-3-100-100-0
gnuplot -e "nc=3" ..\plot-state.gp ..\plot-cables.gp  ..\plot-energy.gp

echo 0-3-1
cd ..\0-3-100-100-1
gnuplot -e "nc=3" ..\plot-state.gp ..\plot-cables.gp  ..\plot-energy.gp

echo 0-4-0
cd ..\0-4-100-100-0
gnuplot -e "nc=4" ..\plot-state.gp ..\plot-cables.gp  ..\plot-energy.gp

echo 0-4-1
cd ..\0-4-100-100-1
gnuplot -e "nc=4" ..\plot-state.gp ..\plot-cables.gp  ..\plot-energy.gp

echo 0-5-0
cd ..\0-5-100-100-0
gnuplot -e "nc=5" ..\plot-state.gp ..\plot-cables.gp  ..\plot-energy.gp

echo 0-5-1
cd ..\0-5-100-100-1
gnuplot -e "nc=5" ..\plot-state.gp ..\plot-cables.gp  ..\plot-energy.gp

echo 0-6-0
cd ..\0-6-100-100-0
gnuplot -e "nc=6" ..\plot-state.gp ..\plot-cables.gp  ..\plot-energy.gp

echo 0-6-1
cd ..\0-6-100-100-1
gnuplot -e "nc=6" ..\plot-state.gp ..\plot-cables.gp  ..\plot-energy.gp
cd ..