$ResourcePkg='./cvAutoTrack.Resource/resource.part01.rar'
$ResourceOutDir='./'+$env:ProjectName+'/'
7z x $ResourcePkg -y -o"$ResourceOutDir"