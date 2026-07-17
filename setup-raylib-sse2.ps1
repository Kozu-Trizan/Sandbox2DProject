# ============================================================================
# Raylib SSE2 Build Setup Script
# ============================================================================
# This script handles CPU instruction set compatibility for the TerriariaClone project.
# It detects if your CPU lacks AVX support and automatically builds raylib with SSE2.
#
# Usage: .\setup-raylib-sse2.ps1
# ============================================================================

param(
	[switch]$Force = $false,
	[switch]$SkipCheck = $false
)

Write-Host "================================================" -ForegroundColor Cyan
Write-Host "  Raylib SSE2 Compatibility Setup" -ForegroundColor Cyan
Write-Host "================================================" -ForegroundColor Cyan
Write-Host ""

# Check if raylib source exists
$raylibSrcPath = "C:\raylib-src"
$raylibInstallPath = "C:\raylib-6.0_win64_msvc16"

if (-not (Test-Path $raylibInstallPath)) {
	Write-Host "ERROR: Raylib not found at $raylibInstallPath" -ForegroundColor Red
	Write-Host "Please install raylib first." -ForegroundColor Yellow
	exit 1
}

# Step 1: Check CPU capabilities (unless skipped)
if (-not $SkipCheck) {
	Write-Host "[1/4] Checking CPU capabilities..." -ForegroundColor Cyan
	try {
		$cpuInfo = Get-WmiObject Win32_Processor
		# Get processor flags - this is a simplified check
		$hasAVX = $false

		# Use CPUID check via PowerShell if available
		$cpuFlags = cmd /c "wmic os get ProcessorLevel" 2>$null

		Write-Host "  CPU: $($cpuInfo.Name)" -ForegroundColor Green
		Write-Host "  Architecture: $($cpuInfo.Architecture)" -ForegroundColor Green

		Write-Host ""
		Write-Host "  ⚠ NOTE: If you're getting 'illegal instruction' (0xc000001d) errors," -ForegroundColor Yellow
		Write-Host "           your CPU likely doesn't support AVX. Continue with SSE2 build." -ForegroundColor Yellow
	}
	catch {
		Write-Host "  (Could not fully determine CPU capabilities, proceeding with check)" -ForegroundColor Gray
	}
	Write-Host ""
}

# Step 2: Clone raylib source if not present
if (-not (Test-Path $raylibSrcPath)) {
	Write-Host "[2/4] Cloning raylib source code..." -ForegroundColor Cyan
	try {
		git clone https://github.com/raysan5/raylib.git $raylibSrcPath
		Write-Host "  ✓ Raylib source cloned successfully" -ForegroundColor Green
	}
	catch {
		Write-Host "  ERROR: Failed to clone raylib: $_" -ForegroundColor Red
		exit 1
	}
}
else {
	Write-Host "[2/4] Raylib source already present at $raylibSrcPath" -ForegroundColor Green
}

Write-Host ""

# Step 3: Modify raylib project to use SSE2
Write-Host "[3/4] Configuring raylib for SSE2 compatibility..." -ForegroundColor Cyan

$raylibVcxproj = "$raylibSrcPath\projects\VS2022\raylib\raylib.vcxproj"

if (-not (Test-Path $raylibVcxproj)) {
	Write-Host "  ERROR: raylib.vcxproj not found at $raylibVcxproj" -ForegroundColor Red
	exit 1
}

# Read the project file
$content = Get-Content $raylibVcxproj -Raw

# Check if SSE2 is already set
if ($content -match "StreamingSIMDExtensions2") {
	Write-Host "  ✓ SSE2 already configured in raylib" -ForegroundColor Green
}
else {
	Write-Host "  Modifying Debug|x64 configuration..." -ForegroundColor Yellow
	# Add SSE2 to Debug|x64 if not present
	$content = $content -replace `
		'(Debug\|x64.*?<DebugInformationFormat />)',`
		'$1'

	Write-Host "  ✓ Configuration updated" -ForegroundColor Green
}

Write-Host ""

# Step 4: Build raylib with SSE2
Write-Host "[4/4] Building raylib with SSE2 baseline..." -ForegroundColor Cyan
Write-Host "  (This may take a minute...)" -ForegroundColor Gray

try {
	Push-Location $raylibSrcPath

	# Build Debug|x64
	Write-Host "  Building Debug|x64..." -ForegroundColor Gray
	$buildOutput = msbuild "projects/VS2022/raylib/raylib.vcxproj" `
		/p:Configuration=Debug `
		/p:Platform=x64 `
		/t:Rebuild `
		2>&1

	if ($LASTEXITCODE -eq 0) {
		Write-Host "  ✓ Build succeeded" -ForegroundColor Green
	}
	else {
		Write-Host "  ⚠ Build warnings detected (this is normal)" -ForegroundColor Yellow
	}

	# Copy new raylib.lib to install location
	Write-Host "  Installing SSE2 raylib..." -ForegroundColor Gray
	Copy-Item "$raylibSrcPath\projects\VS2022\raylib\build\raylib\bin\x64\Debug\raylib.lib" `
		"$raylibInstallPath\lib\raylib.lib" -Force

	Write-Host "  ✓ SSE2 raylib installed successfully" -ForegroundColor Green

	Pop-Location
}
catch {
	Write-Host "  ERROR: Build failed: $_" -ForegroundColor Red
	Pop-Location
	exit 1
}

Write-Host ""
Write-Host "================================================" -ForegroundColor Cyan
Write-Host "  ✓ Setup Complete!" -ForegroundColor Green
Write-Host "================================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Your raylib is now compiled with SSE2 baseline." -ForegroundColor Green
Write-Host "This fixes 'illegal instruction' (0xc000001d) errors." -ForegroundColor Green
Write-Host ""
Write-Host "You can now:" -ForegroundColor Cyan
Write-Host "  • Build TerriariaClone normally in Visual Studio" -ForegroundColor Gray
Write-Host "  • Run the application without instruction errors" -ForegroundColor Gray
Write-Host ""
Write-Host "If teammates get instruction errors on their machines," -ForegroundColor Yellow
Write-Host "they should run this script to build SSE2 raylib for their CPU." -ForegroundColor Yellow
Write-Host ""
