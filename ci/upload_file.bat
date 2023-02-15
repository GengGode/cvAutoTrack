set version=7.5.60

set login_username=GitAction
set login_password=iOpEv21kPOt5ZrMqViLKvJJXshh6olNR
set login_api=/api/auth/login
set login_url=http://download.weixitianli.com%login_api%
curl -X POST %login_url%?Username=%login_username%^&Password=%login_password%
set token=..6tYz-
echo %token%
set token=..aW-
set upload_api=/api/fs/put
set upload_url=http://download.weixitianli.com%upload_api%
set upload_file=cvAutotrack-%version%.zip
(powershell -Command "(Get-Item %upload_file%).Length")>upload_file_length.txt
set /p upload_file_length=<upload_file_length.txt
echo %upload_file_length%

set upload_path=/TianLiUpdateService/cvAutoTrack/%upload_file%
set upload_content_token="Authorization:%token%"
set upload_content_type="Content-Type:multipart/form-data"
set upload_content_length="Content-Length:%upload_file_length%"
set upload_content_path="file-path:%upload_path%"

echo curl -X PUT "%upload_url%" -F file=%upload_file% -H %upload_content_token% -H %upload_content_type% -H %upload_content_length% -H %upload_content_path%
curl -X PUT "%upload_url%" -F file=%upload_file% -H %upload_content_token% -H %upload_content_type% -H %upload_content_length% -H %upload_content_path%

curl -X GET "http://download.weixitianli.com/api/admin/user/list" -H "Authorization:..aW-"

curl -X GET http://download.weixitianli.com/api/admin/user/list -H 'Authorization:..aW-'

