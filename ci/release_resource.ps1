$ResourcePkg='./cvAutoTrack.Resource/resource.7z'
$ResourceOutDir='./'+$env:ProjectName+'/'
7z x $ResourcePkg -y -o"$ResourceOutDir"