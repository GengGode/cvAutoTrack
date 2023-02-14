echo build version.h before build

if exist src\version\Version.h (
	del src\version\Version.h
)
if exist src\version\version.ver (
	del src\version\version.ver
)
if exist src\version\version.branch (
	del src\version\version.branch
)
if exist src\version\version_hash.hash (
	del src\version\version_hash.hash
)
if exist src\version\version_next.number (
	del src\version\version_next.number
)

copy src\version\version_tag.tag src\version\version.ver
git rev-parse --abbrev-ref HEAD>>  src\version\version.branch
git log -n1 --format=format:"%%h">> src\version\version_hash.hash
powershell src\version\GetNextBuildVersion.ps1>> src\version\version_next.number

for /f %%x in (src\version\version.ver) do (
	set version=%%x
)
for /f  "tokens=1,2,3 delims=.-" %%x  in  ("%version%")  do (
	set v1=%%x
	set v2=%%y
	set /a v3=%%z+1
)

set appVeyorV1=0
set appVeyorV2=0
if exist src\version\appVeyor_version_tag.tag (
	for /f "tokens=1,2 delims=.-" %%a in (src\version\appVeyor_version_tag.tag) do (
		set appVeyorV1=%%a
		set appVeyorV2=%%b
	)
)

if not "!appVeyorV1!"=="" (
	if %appVeyorV1% GTR %v1% (
		set /a v1=%appVeyorV1%
		set /a v2=0
		set /a v3=0
	)
)
if not "%appVeyorV2%"=="" (
	if %appVeyorV2% GTR %v2% (
		set /a v2=%appVeyorV2%
		set /a v3=0
	)
)

set /a v4=0
for /f %%x in (src\version\version_next.number) do (
	set /a v4=%%x-1
)
for /f %%x in (src\version\version.branch) do (
	set v5=%%x
)
for /f %%x in (src\version\version_hash.hash) do (
	set v6=%%x
)
chcp 65001
echo #pragma once>>src\version\Version.h
echo namespace TianLi::Version>>src\version\Version.h
echo {>>src\version\Version.h
echo    const int version_major = %v1%;>>src\version\Version.h
echo    const int version_minor = %v2%;>>src\version\Version.h
echo    const int version_revision = %v3%;>>src\version\Version.h
echo    const int version_build = %v4%;>>src\version\Version.h
echo    const std::string version_hash = "%v6%";>>src\version\Version.h
echo    const std::string build_version = "%v1%.%v2%.%v3%.%v4%-%v5%-%v6%";>>src\version\Version.h
echo %#ifndef _DEBUG>>src\version\Version.h
echo    const std::string build_time = "%date%";>>src\version\Version.h
echo #else>>src\version\Version.h
echo    const std::string build_time = "%date% %time%";>>src\version\Version.h
echo %#endif>>src\version\Version.h
echo }>>src\version\Version.h
echo //该文件自动生成，无需更改 >>src\version\Version.h

if exist src\version\version_tag.tag (
	del src\version\version_tag.tag
)

echo %v1%.%v2%.%v3%>>src\version\version_tag.tag

echo build version : %v1%.%v2%.%v3%.%v4%-%v5%-%v6% 
