Remove-Item *.spv

$glslcPath = "C:\VulkanSDK\1.2.182.0\Bin\glslc.exe"

$vertFiles = Get-ChildItem -Path "./*.vert"
foreach ($file in $vertFiles) {
    Write-Output "Compiling $($file.name) to $($file.basename)Vert.spv"
    $command = "$glslcPath $($file.name) -o $($file.basename)Vert.spv"
    Invoke-Expression -Command $command
}

$fragFiles = Get-ChildItem -Path "./*.frag"
foreach ($file in $fragFiles) {
    Write-Output "Compiling $($file.name) to $($file.basename)Frag.spv"
    $command = "$glslcPath $($file.name) -o $($file.basename)Frag.spv"
    Invoke-Expression -Command $command
}