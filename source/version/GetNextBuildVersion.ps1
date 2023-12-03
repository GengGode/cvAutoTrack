$token = '5n6p8sb7j6aavausu020'
$headers = @{}
$headers['Authorization'] = "Bearer $token"
$headers["Content-type"] = "application/json"
$result=$(Invoke-RestMethod -Uri 'https://ci.appveyor.com/api/projects/GengGode/genshinimpact-autotrack-dll/settings' -Headers $headers -Method Get)
echo $result.settings.nextBuildNumber