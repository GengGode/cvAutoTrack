function Get-LoginToken {
  param (
    [string]$username,
    [string]$password
  )
  $login_username = 'GitAction'
  $login_api = '/api/auth/login'
  $login_url = 'https://download.weixitianli.com' + $login_api + '?Username=' + $login_username + '&Password=' + $login_password
  $token = (Invoke-RestMethod -Uri $login_url -Method Post).data.token
  if ($token -eq $null) {
    Write-Host "Login Failed"
  }
  else
  {
    Write-Host "Login Success"
  }
  return $token
}

function Put-FileToAlist {
  param (
    [string]$token,
    [string]$file,
    [string]$target_filename,
    [string[]]$upload_paths
  )
  $upload_api = '/api/fs/put'
  $upload_url = 'https://download.weixitianli.com' + $upload_api
  $upload_file_length = (Get-Item $file).Length

  foreach ($upload_path in $upload_paths) {
    $upload_headers = @{
      Authorization    = $token
      'Content-Type'   = 'application/x-zip-compressed'
      'Content-Length' = $upload_file_length
      'file-path'      = $upload_path + $target_filename
    }

    try {
      $uploadInfo = Invoke-RestMethod -Uri $upload_url -Method Put -Headers $upload_headers -InFile $file
      Write-Host $uploadInfo
      $successTime++
      Write-Host "Upload Success on $target_filename"
    }
    catch {
      Write-Host "Upload Error on $target_filename"
      return $upload_api.code
    }
  }
}
