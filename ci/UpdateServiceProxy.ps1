function GenerateDepsFileJson {
    param (
        [string]$fileName,
        [string]$filePath,
        [string]$downloadUrl,
        [string]$hash
    )
    $json = @" 
    {
        "fileName": "$fileName",
        "filePath": "$filePath",
        "downloadUrl": "$downloadUrl",
        "hash": "$hash"
    }
"@
    return $json
}

function BindDepsFileJson {
    param (
        [string]$depsJson,
        [string]$addJson
    )
    $json = $depsJson + "," + $addJson
}

function GenerateUploadVersionJson {
    param (
        [string]$version,
        [string]$cvAutoTrackHash,
        [string]$log,
        [string]$downloadUrl,
        [string]$depsJson
    )
    $json = @" 
    {
        "version": "$version",
        "downloadUrl": "$downloadUrl",
        "hash": "$cvAutoTrackHash",
        "updateLog": "$log",
        "isDraft": false,
        "files": [
            $depsJson
        ]
    }
"@
    return $json
}

function PostVersion {
    param(
        [string]$token,
        [string]$jsonBody
    )
    $url = 'https://download.api.weixitianli.com/cvAutoTrack.Core/Version?token=' + $token
    $info = Invoke-RestMethod -Method Post -Uri $url -Body $jsonBody -ContentType 'application/json'
    return $info
}

function GenDepsJson{
    param(
        [array]$deps_files,
        [string]$parent_dir_url
    )
    $depsJsons = (New-Object System.Collections.ArrayList)
    foreach ($deps_file in $deps_files) {
        $fileName = $deps_file.Name
        $filePath = "./deps"
        $downloadUrl = $parent_dir_url + $fileName
        $md5_is_exists = Test-Path -Path "$deps_file.md5"
        if($md5_is_exists) {
            $hash = Get-Content -Path "$deps_file.md5"
        }
        else {
            $hash = (Get-FileHash -Path $deps_file -Algorithm MD5).Hash
        }
        $depsJson = GenerateDepsFileJson -fileName $fileName -filePath $filePath -downloadUrl $downloadUrl -hash $hash
        if($depsJsons.Count -ne 0) {
             $depsJsons.Add(",") | Out-Null
        }
        $depsJsons.Add($depsJson) | Out-Null
    }
    return $depsJsons | Out-String
}

function GenVersionPostBody {
    param(
        [string]$version,
        [string]$cvAutoTrackFile,
        [string]$log,
        [string]$downloadUrl,
        [string]$depsJson
    )
    $cvAutoTrackHash = (Get-FileHash -Path $cvAutoTrackFile -Algorithm MD5).Hash
    $json = GenerateUploadVersionJson -version $version -cvAutoTrackHash $cvAutoTrackHash -log $log -downloadUrl $downloadUrl -depsJson $depsJson
    return $json
}
