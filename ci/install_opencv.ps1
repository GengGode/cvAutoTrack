mkdir $OpenCvDir 
wget $OpenCvUrl -OutFile Opencv.zip
7z x ./*.zip -y -o"$OpenCvDir"
rm ./*.zip