$OpenCvDir='C:\opencv\'
$OpenCvUrl='https://github.com/GengGode/OpencvStaticLib/releases/download/4.6.0/Opencv.zip'
mkdir $OpenCvDir 
wget $OpenCvUrl -OutFile Opencv.zip
7z x ./*.zip -y -o"$OpenCvDir"
rm ./*.zip