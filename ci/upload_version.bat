echo 获取版本号

git describe --tags>>version.ver
for /f %%x in (version.ver) do (
	set version=%%x
)
echo %version%
7z a -tZip cvAutoTrack-%version%.zip %MyProjectDir%\cvAutoTrack\x64\Compile\cvAutoTrack.dll
powershell (Get-FileHash -Path cvAutoTrack-$version.zip -Algorithm MD5).Hash>> md5.hash

echo 上传
curl -L "https://github.com/aoaostar/alidrive-uploader/releases/download/v2.2.1/alidrive_uploader_v2.2.1_windows_amd64.zip" -o alidrive.zip 
tar -xf alidrive.zip --strip-components 1 | del example.config.yaml
echo ali_drive:\t\n  drive_id:\t53348742\n  refresh_token:\t7e5bc0b6829c40ac97864d937502447a\n  root_path:\t/TianLiUpdateService\ndebug:\tfalse\nretry:\t3\ntransfers:\t3>>config.yaml 
.\alidrive.exe .\cvAutoTrack-%version%.zip /cvAutoTrack
echo 推送版本
for /f %%x in (version.ver) do (
	set version=%%x
)
for /f %%x in (version.ver) do (
	set buildversion=%%x
)
for /f %%x in (md5.hash) do (
	set md5=%%x
)
git log -1 --pretty=%B>> commit.log
for /f %%x in (md5.hash) do (
	set commitLog=%%x
)

set token=cvAutoTrackTmpToken
set url=http://update.api.weixitianli.com/cvAutoTrack/Version?token=%token%
set body={"version":"%version%","description":"%buildVersion%","downloadUrl":"http://download.weixitianli.com/d/TianLiUpdateService/cvAutoTrack/cvAutoTrack-%version%.zip","hash":"%md5%","updateLog":"%commitLog%"}
echo $url
echo $body
curl -X 'POST' %url% -H 'accept: application/json' -H 'Content-Type: application/json' -d %body%
