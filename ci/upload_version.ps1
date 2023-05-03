Write-Host  获取版本号
git describe --tags | out-file version.ver
$version = Get-Content version.ver
Write-Host  (Get-Content version.ver)
7z a -tZip cvAutoTrack-%APPVEYOR_BUILD_VERSION%.zip $env:MyProjectDir\cvAutoTrack\x64\Compile\cvAutoTrack.dll
(Get-FileHash -Path cvAutoTrack-$version.zip -Algorithm MD5).Hash | out-file md5.hash
Write-Host  (Get-Content md5.hash)

Write-Host  上传
curl -L "https://github.com/aoaostar/alidrive-uploader/releases/download/v2.2.1/alidrive_uploader_v2.2.1_windows_amd64.zip" -o alidrive.zip 
tar -xf alidrive.zip --strip-components 1 | del example.config.yaml
Out-File -FilePath config.yaml -InputObject "ali_drive:`t`n  drive_id:`t53348742`n  refresh_token:`t7e5bc0b6829c40ac97864d937502447a`n  root_path:`t/TianLiUpdateService`ndebug:`tfalse`nretry:`t3`ntransfers:`t3"
.\alidrive.exe .\cvAutoTrack-$version.zip /cvAutoTrack
Write-Host  推送版本
$version = Get-Content version.ver
$buildversion = Get-Content build_version.ver
$md5 = Get-Content md5.hash
$commitLog = git log -1 --pretty=%B

$token="cvAutoTrackTmpToken"
$url='http://update.api.weixitianli.com/cvAutoTrack/Version?token='+$token
$body='{"version":"' + $version+'","description":"' + $buildVersion + '","downloadUrl":"http://download.weixitianli.com/d/TianLiUpdateService/cvAutoTrack/cvAutoTrack-' + $version + '.zip","hash":"' + $md5 + '","updateLog":"' + $commitLog + '"}'
Write-Host  $url
Write-Host  $body
Invoke-RestMethod -Method Post -Uri $url -Body $body -ContentType 'application/json'