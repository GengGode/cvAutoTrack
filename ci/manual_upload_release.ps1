#$version='7.5.116.383-HEAD-85cb2f8'
$md5=(Get-FileHash -Path cvAutoTrack-$version.zip -Algorithm MD5).Hash
$token="cvAutoTrackTmpToken"
$url='http://update.api.weixitianli.com/cvAutoTrack/Version?token='+$token
$body='{"version":"' + $version+'","description":"' + $buildVersion + '","downloadUrl":"http://download.weixitianli.com/d/TianLiUpdateService/cvAutoTrack/cvAutoTrack-' + $version + '.zip","hash":"' + $md5 + '","updateLog":"' + $commitLog + '"}'
Invoke-RestMethod -Method Post -Uri $url -Body $body -ContentType 'application/json'