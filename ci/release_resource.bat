set OpenCvDir=C:\opencv\
set OpenCvUrl=https://github.com/GengGode/OpencvStaticLib/releases/download/4.6.0/Opencv.zip
mkdir %OpenCvDir%
DownloadFile %OpenCvUrl%
7z x ./*.zip -y -o%OpenCvDir%
rm -f ./*.zip