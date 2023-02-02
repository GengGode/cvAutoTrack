$ResourcePkg='./cvAutoTrack.Resource/resource.part01.rar'
$ResourceOutDir='./cvAutoTrack/'
mkdir $ResourceOutDir
git submodule update --recursive
7z x $ResourcePkg -y -o"$ResourceOutDir"