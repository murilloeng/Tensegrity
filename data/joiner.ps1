$dir = Get-Location
$stream_out = New-Object IO.StreamWriter "$dir\$($args[$args.Count - 1])"
$streams_in = $args[0..($args.Count - 2)] | ForEach-Object { New-Object IO.StreamReader "$dir\$_" }

while($true)
{
	$anyLeft = $false
	ForEach ($stream in $streams_in)
	{
		$anyLeft = $anyLeft -or !$stream.EndOfStream
		$stream_out.Write($stream.ReadLine())
	}
	if(!$anyLeft) { break }
	$stream_out.WriteLine()
}

$stream_out.Close()
ForEach ($stream in $streams_in) { $stream.Close() }