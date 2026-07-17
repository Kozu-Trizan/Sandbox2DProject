# CPU Compatibility Setup Guide

## Problem: "Illegal Instruction" Error (0xc000001d)

If you see this error when running TerriariaClone:
```
TerriariaClone.exe (process XXXX) exited with code -1073741795 (0xc000001d).
```

**This means your CPU doesn't support the AVX SIMD instruction set.**

## Solution: Build Raylib with SSE2

Newer CPUs support AVX, but older ones only have SSE2. The raylib binary may have been compiled with AVX. To fix this, rebuild raylib with SSE2 baseline support.

### Quick Setup (Automatic)

Run this PowerShell script from the project root:

```powershell
.\setup-raylib-sse2.ps1
```

**That's it!** The script will:
1. Clone raylib source code (if not already present)
2. Update it for SSE2 compilation
3. Build a compatible raylib.lib
4. Install it to your raylib folder

Then rebuild your project in Visual Studio.

### Manual Setup (If Script Fails)

If you prefer manual control:

```powershell
# 1. Clone raylib source
git clone https://github.com/raysan5/raylib.git C:\raylib-src

# 2. Build with SSE2
cd C:\raylib-src
msbuild projects/VS2022/raylib/raylib.vcxproj /p:Configuration=Debug /p:Platform=x64 /t:Rebuild

# 3. Replace the library
Copy-Item "C:\raylib-src\projects\VS2022\raylib\build\raylib\bin\x64\Debug\raylib.lib" `
	"C:\raylib-6.0_win64_msvc16\lib\raylib.lib" -Force
```

### After Setup

1. Rebuild TerriariaClone in Visual Studio (Ctrl+Shift+B)
2. Run the application - it should work now!

## For Team Members

If a teammate encounters the **0xc000001d error**:

1. They should run `.\setup-raylib-sse2.ps1` once
2. Rebuild their project
3. Done!

This is **not** required if they have a modern CPU with AVX support. The original raylib binary works fine on those machines.

## Technical Details

- **Default raylib**: Compiled with AVX2 instruction set (requires modern CPU)
- **SSE2 raylib**: Compiled with baseline SSE2 support (works on older CPUs)
- **Performance**: Minimal impact - SSE2 is still very efficient
- **Compatibility**: SSE2 raylib works on all Windows x64 CPUs

## Notes

- The setup script creates `C:\raylib-src` if it doesn't exist (only ~200MB)
- Your project configuration stays unchanged - no git conflicts
- Running the script multiple times is safe (it skips if already done)
- The SSE2 raylib is local to your machine and won't affect teammates

---

**Still having issues?** Check that:
- You have Git installed and in PATH
- You have Visual Studio 2022 with C++ workload
- You have admin access to write to `C:\raylib-6.0_win64_msvc16\lib\`
