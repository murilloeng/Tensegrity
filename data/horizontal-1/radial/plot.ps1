$nc = 6
$nr = 10
for($i = 3; $i -le $nc; $i++)
{
	for($j = 0; $j -lt $nr; $j++)
	{
		..\..\joiner.ps1 $i\model-$j-state.txt $i\model-$j-solver.txt $i\model-$j-state-full.txt
		..\..\joiner.ps1 $i\model-$j-cables.txt $i\model-$j-solver.txt $i\model-$j-cables-full.txt
	}
	gnuplot -e "nc=$i; dof_index=1" plot.gp
	gnuplot -e "nc=$i; dof_index=3" plot.gp
	Write-Host "cables: $i done!"
}