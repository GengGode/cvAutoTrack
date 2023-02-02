mkdir $env:OpenCvDir 
wget $env:OpenCvUrl -OutFile Opencv.zip
7z x ./*.zip -y -o"$env:OpenCvDir"
rm ./*.zip