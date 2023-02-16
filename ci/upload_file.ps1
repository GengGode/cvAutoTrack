$version='7.5.60'

$login_username='GitAction'
$login_password='iOpEv21kPOt5ZrMqViLKvJJXshh6olNR'
$login_api='/api/auth/login'
$login_url='http://download.weixitianli.com'+$login_api + '?Username='+$login_username+'&Password='+$login_password
$token=(Invoke-RestMethod -Uri $login_url -Method Post).data.token
echo $token

$upload_api='/api/fs/put'
$upload_url='http://download.weixitianli.com'+$upload_api
$upload_file='cvAutotrack-'+$version+'.zip'
$upload_file_length=(Get-Item $upload_file).Length
$upload_path='/TianLiUpdateService/cvAutoTrack/'+$upload_file
$upload_content_type='multipart/form-data'
$upload_headers=@{
    Authorization = $token
    'Content-Type' = 'multipart/form-data'
    'Content-Length' = $upload_file_length
    'file-path' = $upload_path
}
Invoke-RestMethod -Uri $upload_url -Method Post -Headers $upload_headers -InFile $upload_file
curl POST -H $upload_headers -F file=@$upload_file $upload_url

# bat curl -X GET http://download.weixitianli.com/api/admin/user/list -H 'Authorization:'
$token=
$upload_content_token=@{
    Authorization = $token
}
Invoke-RestMethod -Uri 'http://download.weixitianli.com/api/admin/user/list' -Method Get -Headers $upload_content_token 
