#$version='7.5.116.383-HEAD-85cb2f8'


$login_username='GitAction'
$login_password='iOpEv21kPOt5ZrMqViLKvJJXshh6olNR'
$login_api='/api/auth/login'
$login_url='http://download.weixitianli.com'+$login_api + '?Username='+$login_username+'&Password='+$login_password
$token=(Invoke-RestMethod -Uri $login_url -Method Post).data.token
echo $token

$upload_api='/api/fs/put'
$upload_url='http://download.weixitianli.com'+$upload_api
$upload_file='cvAutoTrack-'+$version+'.zip'
$upload_file_length=(Get-Item $upload_file).Length
$upload_path='/%E5%90%8C%E6%AD%A5%E5%A4%84%E7%90%86/ali4/cvAutoTrack/'+$upload_file
$upload_content_type='multipart/form-data'
$upload_headers=@{
    Authorization = $token
    'Content-Type' = 'application/x-zip-compressed'
    'Content-Length' = $upload_file_length
    'file-path' = $upload_path
}
Invoke-RestMethod -Uri $upload_url -Method Put -Headers $upload_headers -InFile $upload_file

$upload_path='/%E5%90%8C%E6%AD%A5%E5%A4%84%E7%90%86/ali3/cvAutoTrack/'+$upload_file
$upload_content_type='multipart/form-data'
$upload_headers=@{
    Authorization = $token
    'Content-Type' = 'application/x-zip-compressed'
    'Content-Length' = $upload_file_length
    'file-path' = $upload_path
}
Invoke-RestMethod -Uri $upload_url -Method Put -Headers $upload_headers -InFile $upload_file

$upload_path='/%E5%90%8C%E6%AD%A5%E5%A4%84%E7%90%86/ali2/cvAutoTrack/'+$upload_file
$upload_content_type='multipart/form-data'
$upload_headers=@{
    Authorization = $token
    'Content-Type' = 'application/x-zip-compressed'
    'Content-Length' = $upload_file_length
    'file-path' = $upload_path
}
Invoke-RestMethod -Uri $upload_url -Method Put -Headers $upload_headers -InFile $upload_file

$upload_path='/%E5%90%8C%E6%AD%A5%E5%A4%84%E7%90%86/ali1/cvAutoTrack/'+$upload_file
$upload_content_type='multipart/form-data'
$upload_headers=@{
    Authorization = $token
    'Content-Type' = 'application/x-zip-compressed'
    'Content-Length' = $upload_file_length
    'file-path' = $upload_path
}
Invoke-RestMethod -Uri $upload_url -Method Put -Headers $upload_headers -InFile $upload_file

$upload_path='/%E5%90%8C%E6%AD%A5%E5%A4%84%E7%90%86/ali0/cvAutoTrack/'+$upload_file
$upload_content_type='multipart/form-data'
$upload_headers=@{
    Authorization = $token
    'Content-Type' = 'application/x-zip-compressed'
    'Content-Length' = $upload_file_length
    'file-path' = $upload_path
}
Invoke-RestMethod -Uri $upload_url -Method Put -Headers $upload_headers -InFile $upload_file

$upload_path='/%E5%90%8C%E6%AD%A5%E5%A4%84%E7%90%86/ali/cvAutoTrack/'+$upload_file
$upload_content_type='multipart/form-data'
$upload_headers=@{
    Authorization = $token
    'Content-Type' = 'application/x-zip-compressed'
    'Content-Length' = $upload_file_length
    'file-path' = $upload_path
}
Invoke-RestMethod -Uri $upload_url -Method Put -Headers $upload_headers -InFile $upload_file




