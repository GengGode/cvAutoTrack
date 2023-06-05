Write-Host  获取版本号
git describe --tags | out-file version.ver
$version = Get-Content version.ver
Write-Host  (Get-Content version.ver)
7z a -tZip cvAutoTrack-%APPVEYOR_BUILD_VERSION%.zip $env:MyProjectDir\cvAutoTrack\x64\Compile\cvAutoTrack.dll
(Get-FileHash -Path cvAutoTrack-$version.zip -Algorithm MD5).Hash | out-file md5.hash
Write-Host  (Get-Content md5.hash)

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