$version='7.5.107-Test-ab99f5f'
$md5=(Get-FileHash -Path cvAutoTrack-$version.zip -Algorithm MD5).Hash
 $commitLog='test'
$buildversion='test'
$token="cvAutoTrackTmpToken"
 $url='http://update.api.weixitianli.com/cvAutoTrack/Version?token='+$token
$body='{"version":"' + $version+'","description":"' + $buildVersion + '","downloadUrl":"http://download.weixitianli.com/d/TianLiUpdateService/cvAutoTrack/cvAutoTrack-' + $version + '.zip","hash":"' + $md5 + '","updateLog":"' + $commitLog + '"}'
Invoke-RestMethod -Method Post -Uri $url -Body $body -ContentType 'application/json'