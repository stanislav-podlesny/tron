# setup prerequisites
#Requires -Version 7
function echo_command($cmd) {
    Write-Host $cmd -ForegroundColor Cyan
    Invoke-Expression $cmd
}
Write-Host "Prerequisites..." -ForegroundColor Green
if ($IsWindows) {
    # git
    $null = winget list --id git.git
    if (!$?) {echo_command "winget install --id git.git --accept-package-agreements"}
    echo_command "winget upgrade --id git.git"

    # vs code
    $null = winget list --id XP9KHM4BK9FZ7Q
    if (!$?) {echo_command "winget install --id XP9KHM4BK9FZ7Q --accept-package-agreements"}
    echo_command "winget upgrade --id XP9KHM4BK9FZ7Q"

    # cmake
    $null = winget list cmake
    if (!$?) {echo_command "winget install cmake"}
    echo_command "winget upgrade cmake"

    # perl - for running Qt5's init-repository perl script
    $null = winget list perl
    if (!$?) {echo_command "winget install perl"}
    echo_command "winget upgrade perl"
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
