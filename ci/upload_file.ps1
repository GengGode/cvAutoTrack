#$version = '7.9.5-6-g6c9f02b'

$successTime = 0

$login_username = 'GitAction'
$login_password = 'iOpEv21kPOt5ZrMqViLKvJJXshh6olNR'
$login_api = '/api/auth/login'
$login_url = 'http://download.weixitianli.com' + $login_api + '?Username=' + $login_username + '&Password=' + $login_password
$token = (Invoke-RestMethod -Uri $login_url -Method Post).data.token
Write-Host $token

Write-Host  Uploading

$upload_api = '/api/fs/put'
$upload_url = 'http://download.weixitianli.com' + $upload_api
$upload_file = 'cvAutoTrack-' + $version + '.zip'
$upload_file_length = (Get-Item $upload_file).Length
$upload_paths = @('/sync/TianLiUpdateService.tianyi/cvAutoTrack/',
  '/sync/TianLiUpdateService.object/cvAutoTrack/',
  '/sync/TianLiUpdateService.local/cvAutoTrack/')

foreach($upload_path in $upload_paths) {
  $upload_headers = @{
    Authorization    = $token
    'Content-Type'   = 'application/x-zip-compressed'
    'Content-Length' = $upload_file_length
    'file-path'      = $upload_path+$upload_file
  }

  try {
    $uploadInfo = Invoke-RestMethod -Uri $upload_url -Method Put -Headers $upload_headers -InFile $upload_file
    Write-Host $uploadInfo
    $successTime++
    Write-Host "Upload Success on"+$upload_path
  }
  catch {
    Write-Host "Upload Error on"+$upload_path
  }
}

if($successTime -eq 0)
{
  Write-Host "All Task have Failed, Program Stop"
  return 504
}
else {
  return 0
}