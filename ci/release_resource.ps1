$ResourcePkg='./cvAutoTrack.Resource/resource.7z.001'
$ResourceOutDir='./'+$env:ProjectName+'/'
7z x $ResourcePkg -y -o"$ResourceOutDir"