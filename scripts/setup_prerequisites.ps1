# setup prerequisites
Write-Host "Prerequisites..." -ForegroundColor Green
if ($IsWindows) {
    $null = winget list cmake
    if (!$?) {
        $cmd = "winget install cmake"
        Write-Host $cmd -ForegroundColor Cyan
        Invoke-Expression $cmd
    }
    $cmd = "winget upgrade cmake"
    Write-Host $cmd -ForegroundColor Cyan
    Invoke-Expression $cmd
}
elseif ($IsLinux) {
    $installed_packages = apt list --installed 2> $null
    $upgradeable_packages = apt list --upgradeable 2> $null
    function is_package_installed($pkg) {($installed_packages | Select-String "^$pkg/") ? $true : $false}
    function is_package_upgradeable($pkg) {($upgradeable_packages | Select-String "^$pkg/") ? $true : $false}
    
    $packages = "cmake",                # Needed to generate makefiles for this dev environment
                "build-essential",      # gcc, g++, make, C standard lib, dev tools
    
                # From https://doc.qt.io/qt-5/linux-requirements.html
                "libfontconfig1-dev",
                "libfreetype6-dev",
                "libx11-dev",
                "libx11-xcb-dev",
                "libxext-dev",
                "libxfixes-dev",
                "libxi-dev",
                "libxrender-dev",
                "libxcb1-dev",
                "libxcb-glx0-dev",
                "libxcb-keysyms1-dev",
                "libxcb-image0-dev",
                "libxcb-shm0-dev",
                "libxcb-icccm4-dev",
                "libxcb-sync-dev",      # "libxcb-sync0-dev" not found -> use "libxcb-sync-dev"
                "libxcb-xfixes0-dev",
                "libxcb-shape0-dev",
                "libxcb-randr0-dev",
                "libxcb-render-util0-dev",
                # "libxcd-xinerama-dev", # not supported anymore? https://askubuntu.com/questions/5138/how-do-i-best-enable-xinerama
                "libxkbcommon-dev",
                "libxkbcommon-x11-dev"
    $ran_apt_update = $false
    foreach ($pkg in $packages) {
        $installed = is_package_installed($pkg)
        if (!$installed) {
            if (!$ran_apt_update) {
                Write-Host "Updating apt..." -ForegroundColor Green
                bash -c "sudo apt update"
                $ran_apt_update = $true
            }
            Write-Host "Installing $pkg..." -ForegroundColor Green
            bash -c "sudo apt install -y $pkg"
        }
    }
    foreach ($pkg in $packages) {
        $upgradeable = is_package_upgradeable($pkg)
        if ($upgradeable) {
            if (!$ran_apt_update) {
                Write-Host "Updating apt..." -ForegroundColor Green
                bash -c "sudo apt update"
                $ran_apt_update = $true
            }
            Write-Host "Upgrading $pkg..." -ForegroundColor Green
            bash -c "sudo apt upgrade -y $pkg"
        }
    }
}
