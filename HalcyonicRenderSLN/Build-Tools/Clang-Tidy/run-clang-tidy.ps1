<#
.SYNOPSIS
    Compiles or tidies up code from Visual Studio .vcxproj project files.

.DESCRIPTION
    This PowerShell script scans for all .vcxproj Visual Studio projects inside a source directory.
    One or more of these projects will be compiled or tidied up (modernized), using Clang.

.PARAMETER aSolutionsPath
    Alias 'dir'. Source directory to find sln files.
                 Projects will be extracted from each sln.

    Important: You can pass an absolute path to a sln. This way, no file searching will be done, and
               only the projects from this solution file will be taken into account.

.PARAMETER aVcxprojToCompile
    Alias 'proj'. Array of project(s) to compile. If empty, all projects found in solutions are compiled.
    If the -literal switch is present, name is matched exactly. Otherwise, regex matching is used,
    e.g. "msicomp" compiles all projects containing 'msicomp'.

    Absolute disk paths to vcxproj files are accepted.

    Can be passed as comma separated values.

.PARAMETER aVcxprojToIgnore
    Alias 'proj-ignore'. Array of project(s) to ignore, from the matched ones.
    If empty, all already matched projects are compiled.
    If the -literal switch is present, name is matched exactly. Otherwise, regex matching is used,
    e.g. "msicomp" ignores projects containing 'msicomp'.

    Can be passed as comma separated values.

.PARAMETER aVcxprojConfigPlatform
    Alias 'active-config'. The configuration-platform pair, separated by |,
    to be used when processing project files.

    E.g. 'Debug|Win32'.
    If not specified, the first configuration-platform found in the current project is used.

.PARAMETER aCppToCompile
    Alias 'file'. What cpp(s) to compile from the found project(s). If empty, all CPPs are compiled.
    If the -literal switch is present, name is matched exactly. Otherwise, regex matching is used,
    e.g. "table" compiles all CPPs containing 'table'.

    Note: If any headers are given then all translation units that include them will be processed.

.PARAMETER aCppToIgnore
    Alias 'file-ignore'. Array of file(s) to ignore, from the matched ones.
    If empty, all already matched files are compiled.
    If the -literal switch is present, name is matched exactly. Otherwise, regex matching is used,
    e.g. "table" ignores all CPPs containing 'table'.

    Can be passed as comma separated values.

.PARAMETER aUseParallelCompile
    Alias 'parallel'. Switch to run in parallel mode, on all logical CPU cores.

.PARAMETER aContinueOnError
     Alias 'continue'. Switch to continue project compilation even when errors occur.

.PARAMETER aTreatAdditionalIncludesAsSystemIncludes
     Alias 'treat-sai'. Switch to treat project additional include directories as system includes.

.PARAMETER aClangCompileFlags
     Alias 'clang-flags'. Flags given to clang++ when compiling project,
     alongside project-specific defines.

.PARAMETER aDisableNameRegexMatching
     Alias 'literal'. Switch to take project and cpp name filters literally, not by regex matching.

.PARAMETER aTidyFlags
      Alias 'tidy'. If not empty clang-tidy will be called with given flags, instead of clang++.
      The tidy operation is applied to whole translation units, meaning all directory headers
      included in the CPP will be tidied up too. Changes will not be applied, only simulated.

      If aTidyFixFlags is present, it takes precedence over this parameter.

      If '.clang-tidy' value is given, configuration will be read from .clang-tidy file
      in the closest parent directory.

.PARAMETER aTidyFixFlags
      Alias 'tidy-fix'. If not empty clang-tidy will be called with given flags, instead of clang++.
      The tidy operation is applied to whole translation units, meaning all directory headers
      included in the CPP will be tidied up too. Changes will be applied to the file(s).

      If present, this parameter takes precedence over aTidyFlags.

      If '.clang-tidy' value is given, configuration will be read from .clang-tidy file
      in the closest parent directory.

.PARAMETER aAfterTidyFixFormatStyle
      Alias 'format-style'. Used in combination with 'tidy-fix'. If present, clang-tidy will
      also format the fixed file(s), using the specified style.
      Possible values: - not present, no formatting will be done
                       - 'file'
                           Literally 'file', not a placeholder.
                           Uses .clang-format file in the closest parent directory.
                       - 'llvm'
                       - 'google'
                       - 'webkit'
                       - 'mozilla'

.PARAMETER aVisualStudioVersion
      Alias 'vs-ver'. Version of Visual Studio (VC++) installed and that'll be used for
      standard library include directories. E.g. 2017.

      Optional. If not given, it will be inferred based on the project toolset version.

.PARAMETER aVisualStudioSku
      Alias 'vs-sku'. Sku of Visual Studio (VC++) installed and that'll be used for
      standard library include directories. E.g. Professional.

      If not given, the first detected Visual Studio SKU will be used.

.PARAMETER aTidyTreatWarningsAsErrors

.NOTES
    Author: Gabriel Diaconita
#>
#Requires -Version 3
param( [alias("proj")]
       [Parameter(Mandatory=$false, HelpMessage="Filter project(s) to compile/tidy")]
       [string[]] $aVcxprojToCompile

     , [alias("dir")]
       [Parameter(Mandatory=$false, HelpMessage="Source directory for finding solutions; projects will be found from each sln")]
       [string] $aSolutionsPath

     , [alias("proj-ignore")]
       [Parameter(Mandatory=$false, HelpMessage="Specify projects to ignore")]
       [string[]] $aVcxprojToIgnore

     , [alias("active-config")]
       [Parameter(Mandatory=$false, HelpMessage="Config/platform to be used, e.g. Debug|Win32")]
       [string] $aVcxprojConfigPlatform

     , [alias("file")]
       [Parameter(Mandatory=$false, HelpMessage="Filter file(s) to compile/tidy")]
       [string[]] $aCppToCompile

     , [alias("file-ignore")]
       [Parameter(Mandatory=$false, HelpMessage="Specify file(s) to ignore")]
       [string[]] $aCppToIgnore

     , [alias("parallel")]
       [Parameter(Mandatory=$false, HelpMessage="Compile/tidy projects in parallel")]
       [switch]   $aUseParallelCompile

     , [alias("continue")]
       [Parameter(Mandatory=$false, HelpMessage="Allow CPT to continue after encounteringan error")]
       [switch]   $aContinueOnError

     , [alias("treat-sai")]
       [Parameter(Mandatory=$false, HelpMessage="Treat project additional include directories as system includes")]
       [switch]   $aTreatAdditionalIncludesAsSystemIncludes

     , [alias("clang-flags")]
       [Parameter(Mandatory=$false, HelpMessage="Specify compilation flags to CLANG")]
       [string[]] $aClangCompileFlags

     , [alias("literal")]
       [Parameter(Mandatory=$false, HelpMessage="Disable regex matching for all paths received as script parameters")]
       [switch]   $aDisableNameRegexMatching

     , [alias("tidy")]
       [Parameter(Mandatory=$false, HelpMessage="Specify flags to CLANG TIDY")]
       [string]   $aTidyFlags

     , [alias("tidy-fix")]
       [Parameter(Mandatory=$false, HelpMessage="Specify flags to CLANG TIDY & FIX")]
       [string]   $aTidyFixFlags

     , [alias("header-filter")]
       [Parameter(Mandatory=$false, HelpMessage="Enable Clang-Tidy to run on header files")]
       [string]   $aTidyHeaderFilter

     , [alias("format-style")]
       [Parameter(Mandatory=$false, HelpMessage="Used with 'tidy-fix'; tells CLANG TIDY-FIX to also format the fixed file(s)")]
       [string]   $aAfterTidyFixFormatStyle

     , [alias("vs-ver")]
       [Parameter(Mandatory=$false, HelpMessage="Version of Visual Studio toolset to use for loading project")]
       [string]   $aVisualStudioVersion = "2017"

     , [alias("vs-sku")]
       [Parameter(Mandatory=$false, HelpMessage="Edition of Visual Studio toolset to use for loading project")]
       [string]   $aVisualStudioSku

     , [alias("tidy-wae")]
       [Parameter(Mandatory=$false, HelpMessage="Enable Clang-Tidy to treat warnings as errors")]
       [switch]   $aTidyTreatWarningsAsErrors
     )

# System Architecture Constants
# ------------------------------------------------------------------------------------------------

Set-Variable -name kLogicalCoreCount -value $Env:number_of_processors   -option Constant

# ------------------------------------------------------------------------------------------------
# Return Value Constants

Set-Variable -name kScriptFailsExitCode      -value  47                 -option Constant

# ------------------------------------------------------------------------------------------------
# File System Constants

Set-Variable -name kExtensionVcxproj         -value ".vcxproj"          -option Constant
Set-Variable -name kExtensionSolution        -value ".sln"              -option Constant
Set-Variable -name kExtensionClangPch        -value ".clang.pch"        -option Constant
Set-Variable -name kExtensionC               -value ".c"                -option Constant


# ------------------------------------------------------------------------------------------------
# Envinroment Variables for controlling logic

Set-Variable -name kVarEnvClangTidyPath     -value "CLANG_TIDY_PATH"-option Constant

# ------------------------------------------------------------------------------------------------
# Clang-Related Constants

Set-Variable -name kClangFlagSupressLINK    -value @("-fsyntax-only")   -option Constant
Set-Variable -name kClangFlagIncludePch     -value "-include-pch"       -option Constant
Set-Variable -name kClangFlagEmitPch        -value "-emit-pch"          -option Constant
Set-Variable -name kClangFlagMinusO         -value "-o"                 -option Constant

Set-Variable -name kClangDefinePrefix       -value "-D"                 -option Constant
Set-Variable -name kClangFlagNoUnusedArg    -value "-Wno-unused-command-line-argument" `
                                                                        -option Constant
Set-Variable -name kClangFlagNoMsInclude    -value "-Wno-microsoft-include" `
                                                                        -Option Constant
Set-Variable -name kClangFlagFileIsCPP      -value "-x c++"             -option Constant
Set-Variable -name kClangFlagFileIsC        -value "-x c"               -option Constant
Set-Variable -name kClangFlagForceInclude   -value "-include"           -option Constant

#Set-Variable -name kClangCompiler           -value "clang++.exe"        -option Constant

# we may have a custom path for Clang-Tidy. Use it if that's the case.
[string] $customTidyPath = [Environment]::GetEnvironmentVariable($kVarEnvClangTidyPath, "User")
if ($customTidyPath)
{
  Set-Variable -name kClangTidy             -value $customTidyPath      -option Constant
}
else
{
  Set-Variable -name kClangTidy             -value "Build-Tools\Clang-Tidy\clang-tidy.exe"     -option Constant
}

Set-Variable -name kClangTidyFlags            -value @("--")            -option Constant
Set-Variable -name kClangTidyFixFlags         -value @("-quiet"
                                                      ,"-fix-errors"
                                                      , "--")           -option Constant
Set-Variable -name kClangTidyFlagHeaderFilter -value "-header-filter="  -option Constant
Set-Variable -name kClangTidyFlagChecks       -value "-checks="         -option Constant
Set-Variable -name kClangTidyFlagWAE          -value "-warnings-as-errors="         -option Constant
Set-Variable -name kClangTidyUseFile          -value ".clang-tidy"      -option Constant
Set-Variable -name kClangTidyFormatStyle      -value "-format-style="   -option Constant

# ------------------------------------------------------------------------------------------------
# Default install locations of LLVM. If present there, we automatically use it

Set-Variable -name kLLVMInstallLocations    -value @("${Env:ProgramW6432}\LLVM\bin"
                                                    ,"${Env:ProgramFiles(x86)}\LLVM\bin"
                                                    )                   -option Constant

#-------------------------------------------------------------------------------------------------
# Custom Types

Add-Type -TypeDefinition @"
  public enum WorkloadType
  {
    Compile,
    Tidy,
    TidyFix
  }
"@

#Console IO
 # ------------------------------------------------------------------------------------------------
 Function Write-Message([parameter(Mandatory = $true)][string] $msg
     , [Parameter(Mandatory = $true)][System.ConsoleColor] $color)
 {
     $foregroundColor = $host.ui.RawUI.ForegroundColor
     $host.ui.RawUI.ForegroundColor = $color
     Write-Output $msg
     $host.ui.RawUI.ForegroundColor = $foregroundColor
 }
 
 # Writes an error without the verbose PowerShell extra-info (script line location, etc.)
 Function Write-Err([parameter(ValueFromPipeline, Mandatory = $true)][string] $msg)
 {
     Write-Message -msg $msg -color Red
 }
 
 Function Write-Success([parameter(ValueFromPipeline, Mandatory = $true)][string] $msg)
 {
     Write-Message -msg $msg -color Green
 }
 
 Function Write-Array($array, $name)
 {
     Write-Output "$($name):"
     $array | ForEach-Object { Write-Output "  $_" }
     Write-Output "" # empty line separator
 }
 
 Function Write-Verbose-Array($array, $name)
 {
     Write-Verbose "$($name):"
     $array | ForEach-Object { Write-Verbose "  $_" }
     Write-Verbose "" # empty line separator
 }
 
 Function Write-Verbose-Timed([parameter(ValueFromPipeline, Mandatory = $true)][string] $msg)
 {
     Write-Verbose "$([DateTime]::Now.ToString("[HH:mm:ss]")) $msg"
 }
 
 Function Print-InvocationArguments()
 {
     $bParams = $PSCmdlet.MyInvocation.BoundParameters
     if ($bParams)
     {
         [string] $paramStr = "clang-build.ps1 invocation args: `n"
         foreach ($key in $bParams.Keys)
         {
             $paramStr += "  $($key) = $($bParams[$key]) `n"
         }
         Write-Verbose $paramStr
     }
 }
 
 Function Print-CommandParameters([Parameter(Mandatory = $true)][string] $command)
 {
     $params = @()
     foreach ($param in ((Get-Command $command).ParameterSets[0].Parameters))
     {
         if (!$param.HelpMessage)
         {
             continue
         }
 
         $params += New-Object PsObject -Prop @{ "Option" = "-$($param.Aliases[0])"
                                               ; "Description" = $param.HelpMessage
                                               }
     }
 
    $params | Sort-Object -Property "Option" | Out-Default
 }
 
 
 
 # Function that gets the name of a command argument when it is only known by its alias
 # For streamlining purposes, it also accepts the name itself.
 Function Get-CommandParameterName([Parameter(Mandatory = $true)][string] $command
                                  ,[Parameter(Mandatory = $true)][string] $nameOrAlias)
 {
   foreach ($param in ((Get-Command $command).ParameterSets[0].Parameters))
   {
     if ($param.Name    -eq       $nameOrAlias -or
         $param.Aliases -contains $nameOrAlias)
     {
       return $param.Name
     }
   }
   return ""
 }
 
 # File IO
 # ------------------------------------------------------------------------------------------------
 Function Remove-PathTrailingSlash([Parameter(Mandatory = $true)][string] $path)
 {
     return $path -replace '\\$', ''
 }
 
 Function Get-FileDirectory([Parameter(Mandatory = $true)][string] $filePath)
 {
     return ([System.IO.Path]::GetDirectoryName($filePath) + "\")
 }
 
 Function Get-FileName( [Parameter(Mandatory = $false)][string] $path
                      , [Parameter(Mandatory = $false)][switch] $noext)
 {
     if ($noext)
     {
         return ([System.IO.Path]::GetFileNameWithoutExtension($path))
     }
     else
     {
         return ([System.IO.Path]::GetFileName($path))
     }
 }
 
 Function IsFileMatchingName( [Parameter(Mandatory = $true)][string] $filePath
     , [Parameter(Mandatory = $true)][string] $matchName)
 {
     if ([System.IO.Path]::IsPathRooted($matchName))
     {
         return $filePath -ieq $matchName
     }
 
     if ($aDisableNameRegexMatching)
     {
         [string] $fileName      = (Get-FileName -path $filePath)
         [string] $fileNameNoExt = (Get-FileName -path $filePath -noext)
         return (($fileName -ieq $matchName) -or ($fileNameNoExt -ieq $matchName))
     }
     else
     {
         return $filePath -match $matchName
     }
 }
 
 Function FileHasExtension( [Parameter(Mandatory = $true)][string]   $filePath
                          , [Parameter(Mandatory = $true)][string[]] $ext
                          )
 {
     foreach ($e in $ext)
     {
         if ($filePath.EndsWith($e))
         {
             return $true
         }
     }
     return $false
 }
 
 <#
   .DESCRIPTION
   Merges an absolute and a relative file path.
   .EXAMPLE
   Having base = C:\Windows\System32 and child = .. we get C:\Windows
   .EXAMPLE
   Having base = C:\Windows\System32 and child = ..\..\..\.. we get C:\ (cannot go further up)
   .PARAMETER base
   The absolute path from which we start.
   .PARAMETER child
   The relative path to be merged into base.
   .PARAMETER ignoreErrors
   If this switch is not present, an error will be triggered if the resulting path
   is not present on disk (e.g. c:\Windows\System33).
 
   If present and the resulting path does not exist, the function returns an empty string.
   #>
 Function Canonize-Path( [Parameter(Mandatory = $true)][string] $base
     , [Parameter(Mandatory = $true)][string] $child
     , [switch] $ignoreErrors)
 {
     [string] $errorAction = If ($ignoreErrors) {"SilentlyContinue"} Else {"Stop"}
 
     if ([System.IO.Path]::IsPathRooted($child))
     {
         if (!(Test-Path $child))
         {
             return ""
         }
         return $child
     }
     else
     {
         [string[]] $paths = Join-Path -Path "$base" -ChildPath "$child" -Resolve -ErrorAction $errorAction
         return $paths
     }
 }
 
 function HasTrailingSlash([Parameter(Mandatory = $true)][string] $str)
 {
     return $str.EndsWith('\') -or $str.EndsWith('/')
 }
 
 
 function EnsureTrailingSlash([Parameter(Mandatory = $true)][string] $str)
 {
     [string] $ret = If (HasTrailingSlash($str)) { $str } else { "$str\" }
     return $ret
 }
 
 function Exists([Parameter(Mandatory = $false)][string] $path)
 {
     if ([string]::IsNullOrEmpty($path))
     {
         return $false
     }
 
     return Test-Path $path
 }
 
 function MakePathRelative( [Parameter(Mandatory = $true)][string] $base
                          , [Parameter(Mandatory = $true)][string] $target
                          )
 {
     Push-Location "$base\"
     [string] $relativePath = (Resolve-Path -Relative $target) -replace '^\.\\',''
     Pop-Location
     if ( (HasTrailingSlash $target) -or $target.EndsWith('.') )
     {
         $relativePath += '\'
     }
     return "$relativePath"
 }
 
 # Command IO
 # ------------------------------------------------------------------------------------------------
 Function Exists-Command([Parameter(Mandatory = $true)][string] $command)
 {
     try
     {
         Get-Command -name $command -ErrorAction Stop | out-null
         return $true
     }
     catch
     {
         return $false
     }
 }
 
 # ------------------------------------------------------------------------------------------------
  # Helpers for locating Visual Studio on the computer
  
  # VsWhere is available starting with Visual Studio 2017 version 15.2.
  Set-Variable -name   kVsWhereLocation `
      -value  "${Env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" #`
  #-option Constant
  
  # Default installation path of Visual Studio 2017. We'll use when VsWhere isn't available.
  Set-Variable -name   kVs15DefaultLocation `
      -value  "${Env:ProgramFiles(x86)}\Microsoft Visual Studio\$global:cptVisualStudioVersion\$aVisualStudioSku" #`
  #-option Constant
  
  # Registry key containing information about Visual Studio 2015 installation path.
  Set-Variable -name   kVs2015RegistryKey `
      -value  "HKLM:SOFTWARE\Wow6432Node\Microsoft\VisualStudio\14.0" #`
  #-option Constant
  
  # Default location for v140 toolset when installed as a feature of a VS 2017 installation
  Set-Variable -name   kVs2017Toolset140DiskLocation `
      -value  "${Env:ProgramFiles(x86)}\Microsoft Visual Studio 14.0" #`
  #-option Constant
  
  Function Get-MscVer()
  {
      return ((Get-Item "$(Get-VisualStudio-Path)\VC\Tools\MSVC\" | Get-ChildItem) | select -last 1).Name
  }
  
  Function Get-VisualStudio-Includes([Parameter(Mandatory = $true)][string]  $vsPath,
      [Parameter(Mandatory = $false)][string] $mscVer)
  {
      [string] $mscVerToken = ""
      If (![string]::IsNullOrEmpty($mscVer))
      {
          $mscVerToken = "Tools\MSVC\$mscVer\"
      }
  
      return @( "$vsPath\VC\$($mscVerToken)include"
          , "$vsPath\VC\$($mscVerToken)atlmfc\include"
      )
  }
  
  Function Get-VisualStudio-Path()
  {
      if ($global:cptVisualStudioVersion -eq "2015")
      {
          # try to detect full installation
          [string] $installLocation = (Get-Item $kVs2015RegistryKey).GetValue("InstallDir")
          if ($installLocation)
          {
              $installLocation = Canonize-Path -base $installLocation -child "..\.." -ignoreErrors
          }
          if ($installLocation)
          {
              return $installLocation
          }
  
          # we may have a VS 2017 installation with v140 toolset feature
          [string] $iostreamLocation = Canonize-Path -base $kVs2017Toolset140DiskLocation `
                                                     -child "VC\include\iostream" -ignoreErrors
          if ($iostreamLocation)
          {
              return $kVs2017Toolset140DiskLocation
          }
  
          Write-Err "Visual Studio 2015 installation location could not be detected"
      }
      else
      {
          if (Test-Path $kVsWhereLocation)
          {
  
              [string] $product = "*"
              if (![string]::IsNullOrEmpty($aVisualStudioSku))
              {
                $product = "Microsoft.VisualStudio.Product.$aVisualStudioSku"
              }
  
              [string[]] $output = (& "$kVsWhereLocation" -nologo `
                                                          -property installationPath `
                                                          -products $product `
                                                          -prerelease)
  
              # the -prerelease switch is not available on older VS2017 versions
              if (($output -join "").Contains("0x57")) <# error code for unknown parameter #>
              {
                  $output = (& "$kVsWhereLocation" -nologo `
                                                   -property installationPath `
                                                   -products $product)
              }
  
              return $output[0]
          }
  
          if (Test-Path -Path $kVs15DefaultLocation)
          {
              return $kVs15DefaultLocation
          }
  
          throw "Cannot locate Visual Studio location"
      }
  }
  
  # REQUIRES io.ps1 to be included

Set-Variable -name "kMsbuildExpressionToPsRules" <#-option Constant#>     `
-value @(                                                             `
    <# backticks are control characters in PS, replace them #>        `
      ('`'                               , ''''                      )`
    <# Temporarily replace $( #>                                      `
    , ('\$\s*\('                         , '!@#'                     )`
    <# Escape $ #>                                                    `
    , ('\$'                              , '`$'                      )`
    <# Put back $( #>                                                 `
    , ('!@#'                             , '$('                      )`
    <# Various operators #>                                           `
    , ("([\s\)\'""])!="                  , '$1  -ne '                )`
    , ("([\s\)\'""])<="                  , '$1  -le '                )`
    , ("([\s\)\'""])>="                  , '$1  -ge '                )`
    , ("([\s\)\'""])=="                  , '$1  -eq '                )`
    , ("([\s\)\'""])<"                   , '$1 -lt '                 )`
    , ("([\s\)\'""])>"                   , '$1 -gt '                 )`
    , ("([\s\)\'""])or"                  , '$1 -or '                 )`
    , ("([\s\)\'""])and"                 , '$1 -and '                )`
    <# Use only double quotes #>                                      `
    , ("\'"                              , '"'                       )`
    , ("Exists\((.*?)\)(\s|$)"           , '(Exists($1))$2'          )`
    , ("HasTrailingSlash\((.*?)\)(\s|$)" , '(HasTrailingSlash($1))$2')`
    , ("(\`$\()(Registry:)(.*?)(\))"     , '$$(GetRegValue("$3"))'   )`
    , ("\[MSBuild\]::GetDirectoryNameOfFileAbove\((.+?),\s*`"?'?((\$.+?\))|(.+?))((|`"|')\))+"`
    ,'GetDirNameOfFileAbove -startDir $1 -targetFile ''$2'')'        )`
    , ("\[MSBuild\]::MakeRelative\((.+?),\s*""?'?((\$.+?\))|(.+?))((|""|')\)\))+"`
    ,'MakePathRelative -base $1 -target "$2")'                       )`
    , ('SearchOption\.', '[System.IO.SearchOption]::'                )`
    , ("@\((.*?)\)", '$(Get-Project-Item("$1"))')`
)

Set-Variable -name "kMsbuildConditionToPsRules" <#-option Constant#>      `
         -value   @(<# Use only double quotes #>                      `
                   ,("\'"                , '"'                       )`
)

function GetDirNameOfFileAbove( [Parameter(Mandatory = $true)][string] $startDir
                          , [Parameter(Mandatory = $true)][string] $targetFile
                          )
{
if ($targetFile.Contains('$'))
{
    $targetFile = Invoke-Expression $targetFile
}

[string] $base = $startDir
while ([string]::IsNullOrEmpty((Canonize-Path -base  $base        `
                -child $targetFile  `
                -ignoreErrors)))
{
    $base = [System.IO.Path]::GetDirectoryName($base)
    if ([string]::IsNullOrEmpty($base))
    {
        return ""
    }
}
return $base
}

function GetRegValue([Parameter(Mandatory = $true)][string] $regPath)
{
Write-Debug "REG_READ $regPath"

[int] $separatorIndex = $regPath.IndexOf('@')
[string] $valueName = ""
if ($separatorIndex -gt 0)
{
    [string] $valueName = $regPath.Substring($separatorIndex + 1)
    $regPath = $regPath.Substring(0, $separatorIndex)
}
if ([string]::IsNullOrEmpty($valueName))
{
    throw "Cannot retrieve an empty registry value"
}
$regPath = $regPath -replace "HKEY_LOCAL_MACHINE\\", "HKLM:\"

if (Test-Path $regPath)
{
    return (Get-Item $regPath).GetValue($valueName)
}
else
{
    return ""
}
}

function Evaluate-MSBuildExpression([string] $expression, [switch] $isCondition)
{
Write-Debug "Start evaluate MSBuild expression $expression"

foreach ($rule in $kMsbuildExpressionToPsRules)
{
    $expression = $expression -replace $rule[0], $rule[1]
}

if ( !$isCondition -and ($expression.IndexOf('$') -lt 0))
{
    # we can stop here, further processing is not required
    return $expression
}

[int] $expressionStartIndex = -1
[int] $openParantheses = 0
for ([int] $i = 0; $i -lt $expression.Length; $i += 1)
{
    if ($expression[$i] -eq '(')
    {
        if ($i -gt 0 -and $expressionStartIndex -lt 0 -and $expression[$i - 1] -eq '$')
        {
            $expressionStartIndex = $i - 1
        }

        if ($expressionStartIndex -ge 0)
        {
            $openParantheses += 1
        }
    }

    if ($expression[$i] -eq ')' -and $expressionStartIndex -ge 0)
    {
        $openParantheses -= 1
        if ($openParantheses -lt 0)
        {
            throw "Parse error"
        }
        if ($openParantheses -eq 0)
        {
            [string] $content = $expression.Substring($expressionStartIndex + 2,
                $i - $expressionStartIndex - 2)
            [int] $initialLength = $content.Length

            if ([regex]::Match($content, "[a-zA-Z_][a-zA-Z0-9_\-]+").Value -eq $content)
            {
                # we have a plain property retrieval
                $content = "`${$content}"
            }
            else
            {
                # dealing with a more complex expression
                $content = $content -replace '(^|\s+|\$\()([a-zA-Z_][a-zA-Z0-9_]+)(\.|\)|$)', '$1$$$2$3'
            }

            $newCond = $expression.Substring(0, $expressionStartIndex + 2) +
            $content + $expression.Substring($i)
            $expression = $newCond

            $i += ($content.Length - $initialLength)
            $expressionStartIndex = -1
        }
    }
}

Write-Debug "Intermediate PS expression: $expression"

try
{
    # try to get actual objects, if possible
    $res = Invoke-Expression $expression
}
catch
{
    # safe-approach first, string expansion
    $res = $ExecutionContext.InvokeCommand.ExpandString($expression)

    Write-Debug $_.Exception.Message
}

Write-Debug "Evaluated expression to: $res"

return $res
}
function Evaluate-MSBuildCondition([Parameter(Mandatory = $true)][string] $condition)
{
Write-Debug "Evaluating condition $condition"
foreach ($rule in $kMsbuildConditionToPsRules)
{
    $condition = $condition -replace $rule[0], $rule[1]
}
[string] $expression = Evaluate-MSBuildExpression -expression $condition -isCondition

if ($expression -ieq "true")
{
    return $true
}

if ($expression -ieq "false")
{
    return $false
}

[bool] $res = $false
try
{
    $res = (Invoke-Expression $expression) -eq $true
}
catch
{
    Write-Debug $_.Exception.Message
}
Write-Debug "Evaluated condition to $res"

return $res
}

  #-------------------------------------------------------------------------------------------------
# Global variables

# vcxproj and property sheet files declare MsBuild properties (e.g. $(MYPROP)).
# they are used in project xml nodes expressions. we have a
# translation engine (MSBUILD-POWERSHELL) for these. it relies on
# PowerShell to evaluate these expressions. We have to inject project
# properties in the PowerShell runtime context. We keep track of them in
# this list, so that each project can know to clean previous vars before loading begins.
if (! (Test-Path variable:global:ProjectSpecificVariables))
{
  [System.Collections.ArrayList] $global:ProjectSpecificVariables    = @()
}

if (! (Test-Path variable:global:ScriptParameterBackupValues))
{
  [System.Collections.Hashtable] $global:ScriptParameterBackupValues = @{}
}

# current vcxproj and property sheets
[xml[]]  $global:projectFiles                    = @();

# path of current project
[string] $global:vcxprojPath                     = "";

# namespace of current project vcxproj XML
[System.Xml.XmlNamespaceManager] $global:xpathNS = $null;


Set-Variable -name "kRedundantSeparatorsReplaceRules" -option Constant `
              -value @( <# handle multiple consecutive separators #>   `
                        (";+" , ";")                                   `
                        <# handle separator at end                #>   `
                      , (";$" , "")                                    `
                        <# handle separator at beginning          #>   `
                      , ("^;" , "")                                    `
                      )

Function Set-Var([parameter(Mandatory = $false)][string] $name
                ,[parameter(Mandatory = $false)]         $value
                ,[parameter(Mandatory = $false)][switch] $asScriptParameter
                )
{
    if ($asScriptParameter)
    {
        if (Test-Path "variable:$name")
        {
          $oldVar = Get-Variable $name
          $oldValue = $oldVar.Value

          if ($oldValue           -and
              $oldValue.GetType() -and
              $oldValue.GetType().ToString() -eq "System.Management.Automation.SwitchParameter")
          {
            $oldValue = $oldValue.ToBool()
          }

          $global:ScriptParameterBackupValues[$name] = $oldValue
        }
        else
        {
          $global:ScriptParameterBackupValues[$name] = $null
        }
    }

    Write-Verbose "SET_VAR $($name): $value"
    if ($asScriptParameter)
    {
      Set-Variable -name $name -Value $value -Scope Script
    }
    else
    {
      Set-Variable -name $name -Value $value -Scope Global
    }

    if (!$asScriptParameter -and !$global:ProjectSpecificVariables.Contains($name))
    {
        $global:ProjectSpecificVariables.Add($name) | Out-Null
    }
}

Function Add-Project-Item([parameter(Mandatory = $false)][string] $name
                         ,[parameter(Mandatory = $false)]         $value)
{
    if (!$value)
    {
        return
    }

    $itemVarName = "CPT_PROJITEM_$name"
    if (!(Get-Variable $itemVarName -ErrorAction SilentlyContinue))
    {
        $itemList = New-Object System.Collections.ArrayList
        Set-Var -name $itemVarName -value $itemList
    }

    $itemList = (Get-Variable $itemVarName).Value
    if ($value.GetType().Name -ieq "object[]")
    {
        foreach ($arrayValue in $value)
        {
            $itemList.Add($arrayValue) | Out-Null
        }
    }
    else
    {
        $itemList.Add($value) | Out-Null
    }
}

Function Get-Project-Item([parameter(Mandatory = $true)][string] $name)
{
    $itemVarName = "CPT_PROJITEM_$name"

    $itemVar = Get-Variable $itemVarName -ErrorAction SilentlyContinue
    if ($itemVar)
    {
        $retStr = ""

        if ($itemVar.Value.GetType().Name -ieq "ArrayList")
        {
            foreach ($v in $itemVar.Value)
            {
                if ($retStr)
                {
                    $retStr += ";"
                }
                $retStr += $v
            }
        }
        else
        {
            $retStr = $itemVar.Value
        }

        return $retStr
    }

    return $null
}

Function Clear-Vars()
{
    Write-Verbose-Array -array $global:ProjectSpecificVariables `
        -name "Deleting variables initialized by previous project"

    foreach ($var in $global:ProjectSpecificVariables)
    {
        Remove-Variable -name $var -scope Global -ErrorAction SilentlyContinue
    }

    foreach ($varName in $global:ScriptParameterBackupValues.Keys)
    {
        Write-Verbose "Restoring $varName to old value $($ScriptParameterBackupValues[$varName])"
        Set-Variable -name $varName -value $ScriptParameterBackupValues[$varName]
    }

    $global:ScriptParameterBackupValues.Clear()

    $global:ProjectSpecificVariables.Clear()
}

Function UpdateScriptParameter([Parameter(Mandatory = $true)] [string] $paramName
                              ,[Parameter(Mandatory = $false)][string] $paramValue)
{
  [bool]   $isSwitch  = $false
  $evalParamValue     = "" # no type specified because we don't know it yet

  if ($paramValue) # a parameter
  {
    $evalParamValue = Invoke-Expression $paramValue # evaluate expression to get actual value
  }
  else # a switch
  {
    $isSwitch = $true
  }

  # the parameter name we detected may be an alias => translate it into the real name
  [string] $realParamName = Get-CommandParameterName -command "$PSScriptRoot\..\clang-build.ps1" `
                                                     -nameOrAlias $paramName
  if (!$realParamName)
  {
    Write-Output "OVERVIEW: Clang Power Tools: compiles or tidies up code from Visual Studio .vcxproj project files`n"

    Write-Output "USAGE: clang-build.ps1 [options]`n"

    Write-Output "OPTIONS: "
    Print-CommandParameters "$PSScriptRoot\..\clang-build.ps1"

    Fail-Script "Unsupported option '$paramName'. Check cpt.config."
  }

  if ($isSwitch)
  {
    Set-Var -name $realParamName -value $true -asScriptParameter
  }
  else
  {
    Set-Var -name $realParamName -value $evalParamValue -asScriptParameter
  }
}

Function Get-ConfigFileParameters()
{
  [System.Collections.Hashtable] $retArgs = @{}

  [string] $startDir = If ([string]::IsNullOrWhiteSpace($ProjectDir)) { $aSolutionsPath } else { $ProjectDir }
  [string] $configFile = (GetDirNameOfFileAbove -startDir $startDir -targetFile "cpt.config") + "\cpt.config"
  if (!(Test-Path $configFile))
  {
      return $retArgs
  }
  Write-Verbose "Found cpt.config in $configFile"

  [xml] $configXml = Get-Content $configFile
  $configXpathNS= New-Object System.Xml.XmlNamespaceManager($configXml.NameTable)
  $configXpathNS.AddNamespace("ns", $configXml.DocumentElement.NamespaceURI)

  [System.Xml.XmlElement[]] $argElems = $configXml.SelectNodes("/ns:cpt-config/*", $configXpathNS)

  foreach ($argEl in $argElems)
  {
    if ($argEl.Name.StartsWith("vsx-"))
    {
        continue # settings for the Visual Studio Extension
    }

    if ($argEl.HasAttribute("Condition"))
    {
      [bool] $isApplicable = Evaluate-MSBuildCondition -condition $argEl.GetAttribute("Condition")
      if (!$isApplicable)
      {
        continue
      }
    }
    $retArgs[$argEl.Name] = $argEl.InnerText
  }

  return $retArgs
}

Function Update-ParametersFromConfigFile()
{
  [System.Collections.Hashtable] $configParams = Get-ConfigFileParameters
  if (!$configParams)
  {
      return
  }

  foreach ($paramName in $configParams.Keys)
  {
    UpdateScriptParameter -paramName $paramName -paramValue $configParams[$paramName]
  }
}

Function InitializeMsBuildProjectProperties()
{
    Write-Verbose "Importing environment variables into current scope"
    foreach ($var in (Get-ChildItem Env:))
    {
        Set-Var -name $var.Name -value $var.Value
    }

    Set-Var -name "MSBuildProjectFullPath"   -value $global:vcxprojPath
    Set-Var -name "ProjectDir"               -value (Get-FileDirectory -filePath $global:vcxprojPath)
    Set-Var -name "MSBuildProjectExtension"  -value ([IO.Path]::GetExtension($global:vcxprojPath))
    Set-Var -name "MSBuildProjectFile"       -value (Get-FileName -path $global:vcxprojPath)
    Set-Var -name "MSBuildProjectName"       -value (Get-FileName -path $global:vcxprojPath -noext)
    Set-Var -name "MSBuildProjectDirectory"  -value (Get-FileDirectory -filePath $global:vcxprojPath)
    Set-Var -name "MSBuildProgramFiles32"    -value "${Env:ProgramFiles(x86)}"
    # defaults for projectname and targetname, may be overriden by project settings
    Set-Var -name "ProjectName"              -value $MSBuildProjectName
    Set-Var -name "TargetName"               -value $MSBuildProjectName

    # These would enable full project platform references parsing, experimental right now
    if ($env:CPT_LOAD_ALL -eq '1')
    {
        Set-Var -name "ConfigurationType"        -value "Application"
        Set-Var -name "VCTargetsPath"            -value "$(Get-VisualStudio-Path)\Common7\IDE\VC\VCTargets\"
        Set-Var -name "VsInstallRoot"            -value (Get-VisualStudio-Path)
        Set-Var -name "MSBuildExtensionsPath"    -value "$(Get-VisualStudio-Path)\MSBuild"
        Set-Var -name "LocalAppData"             -value $env:LOCALAPPDATA
        Set-Var -name "UserRootDir"              -value "$LocalAppData\Microsoft\MSBuild\v4.0"
        Set-Var -name "UniversalCRT_IncludePath" -value "${Env:ProgramFiles(x86)}\Windows Kits\10\Include\10.0.10240.0\ucrt"
    }

    [string] $vsVer = "15.0"
    if ($global:cptVisualStudioVersion -eq "2015")
    {
        $vsVer = "14.0"
    }
    Set-Var -name "VisualStudioVersion"    -value $vsVer
    Set-Var -name "MSBuildToolsVersion"    -value $vsVer

    [string] $projectSlnPath = Get-ProjectSolution
    [string] $projectSlnDir = Get-FileDirectory -filePath $projectSlnPath
    Set-Var -name "SolutionDir" -value $projectSlnDir
    [string] $projectSlnName = Get-FileName -path $projectSlnPath -noext
    Set-Var -name "SolutionName" -value $projectSlnName

    Update-ParametersFromConfigFile
}

Function InitializeMsBuildCurrentFileProperties([Parameter(Mandatory = $true)][string] $filePath)
{
    Set-Var -name "MSBuildThisFileFullPath"  -value $filePath
    Set-Var -name "MSBuildThisFileExtension" -value ([IO.Path]::GetExtension($filePath))
    Set-Var -name "MSBuildThisFile"          -value (Get-FileName -path $filePath)
    Set-Var -name "MSBuildThisFileName"      -value (Get-FileName -path $filePath -noext)
    Set-Var -name "MSBuildThisFileDirectory" -value (Get-FileDirectory -filePath $filePath)
}

<#
.DESCRIPTION
A wrapper over the XmlDOcument.SelectNodes function. For convenience.
Not to be used directly. Please use Select-ProjectNodes instead.
#>
function Help:Get-ProjectFileNodes([xml] $projectFile, [string] $xpath)
{
    [System.Xml.XmlElement[]] $nodes = $projectFile.SelectNodes($xpath, $global:xpathNS)
    return $nodes
}

function  GetNodeInheritanceToken([System.Xml.XmlNode] $node)
{
    [string] $inheritanceToken = "%($($node.Name))";
    if ($node.InnerText.Contains($inheritanceToken))
    {
        return $inheritanceToken
    }

    return ""
}

function ReplaceInheritedNodeValue([System.Xml.XmlNode] $currentNode
    , [System.Xml.XmlNode] $nodeToInheritFrom
)
{
    [string] $inheritanceToken = GetNodeInheritanceToken($currentNode)
    if ([string]::IsNullOrEmpty($inheritanceToken))
    {
        # no need to inherit
        return $false
    }

    [string] $replaceWith = ""
    if ($nodeToInheritFrom)
    {
        $replaceWith = $nodeToInheritFrom.InnerText
    }

    [string] $whatToReplace = [regex]::Escape($inheritanceToken);
    if ([string]::IsNullOrEmpty($replaceWith))
    {
        # handle semicolon separators
        [string] $escTok = [regex]::Escape($inheritanceToken)
        $whatToReplace = "(;$escTok)|($escTok;)|($escTok)"
    }

    # replace inherited token and redundant separators
    $replacementRules = @(, ($whatToReplace, $replaceWith)) + $kRedundantSeparatorsReplaceRules
    foreach ($rule in $replacementRules)
    {
        $currentNode.InnerText = $currentNode.InnerText -replace $rule[0], $rule[1]
    }

    return $currentNode.InnerText.Contains($inheritanceToken)
}

<#
.SYNOPSIS
Selects one or more nodes from the project.
.DESCRIPTION
We often need to access data from the project, e.g. additional includes, Win SDK version.
A naive implementation would be to simply look inside the vcxproj, but that leaves out
property sheets.

This function takes care to retrieve the nodes we're searching by looking in both the .vcxproj
and property sheets, taking care to inherit values accordingly.
.EXAMPLE
Give an example of how to use it
.EXAMPLE
Give another example of how to use it.
.PARAMETER xpath
XPath we want to use for searching nodes.
.PARAMETER fileIndex
Optional. Index of the project xml file we want to start our search in.
0 = .vcxproj and then, recursively, all property sheets
1 = first property sheet and then, recursively, all other property sheets
etc.
#>
function Select-ProjectNodes([Parameter(Mandatory = $true)]  [string][string] $xpath
    , [Parameter(Mandatory = $false)] [int]            $fileIndex = 0)
{
    [System.Xml.XmlElement[]] $nodes = @()

    if ($fileIndex -ge $global:projectFiles.Count)
    {
        return $nodes
    }

    $nodes = Help:Get-ProjectFileNodes -projectFile $global:projectFiles[$fileIndex] -xpath $xpath

    # nothing on this level or we're dealing with an ItemGroup, go above
    if ($nodes.Count -eq 0 -or $xpath.Contains("ItemGroup"))
    {
        [System.Xml.XmlElement[]] $upperNodes = Select-ProjectNodes -xpath $xpath -fileIndex ($fileIndex + 1)
        if ($upperNodes.Count -gt 0)
        {
            $nodes += $upperNodes
        }
        return $nodes
    }

    if ($nodes[$nodes.Count - 1]."#text")
    {
        # we found textual settings that can be inherited. see if we should inherit

        [System.Xml.XmlNode] $nodeToReturn = $nodes[$nodes.Count - 1]
        if ($nodeToReturn.Attributes.Count -gt 0)
        {
            throw "Did not expect node to have attributes"
        }

        [bool] $shouldInheritMore = ![string]::IsNullOrEmpty((GetNodeInheritanceToken -node $nodeToReturn))
        for ([int] $i = $nodes.Count - 2; ($i -ge 0) -and $shouldInheritMore; $i -= 1)
        {
            $shouldInheritMore = ReplaceInheritedNodeValue -currentNode $nodeToReturn -nodeToInheritFrom $nodes[$i]
        }

        if ($shouldInheritMore)
        {
            [System.Xml.XmlElement[]] $inheritedNodes = Select-ProjectNodes -xpath $xpath -fileIndex ($fileIndex + 1)
            if ($inheritedNodes.Count -gt 1)
            {
                throw "Did not expect to inherit more than one node"
            }
            if ($inheritedNodes.Count -eq 1)
            {
                $shouldInheritMore = ReplaceInheritedNodeValue -currentNode $nodeToReturn -nodeToInheritFrom $inheritedNodes[0]
            }
        }

        # we still could have to inherit from parents but when not loading
        # all MS prop sheets we have nothing to inherit from, delete inheritance token
        ReplaceInheritedNodeValue -currentNode $nodeToReturn -nodeToInheritFrom $null | Out-Null

        return @($nodeToReturn)
    }
    else
    {
        # return what we found
        return $nodes
    }
}

<#
.DESCRIPTION
   Finds the first config-platform pair in the vcxproj.
   We'll use it for all project data retrievals.

   Items for other config-platform pairs will be removed from the DOM.
   This is needed so that our XPath selectors don't get confused when looking for data.
#>
function Detect-ProjectDefaultConfigPlatform([string] $projectValue)
{
    [string]$configPlatformName = ""

    if (![string]::IsNullOrEmpty($aVcxprojConfigPlatform))
    {
        $configPlatformName = $aVcxprojConfigPlatform
    }
    else
    {
        $configPlatformName = $projectValue
    }

    if ([string]::IsNullOrEmpty($configPlatformName))
    {
        throw "Could not automatically detect a configuration platform"
    }

    [string[]] $configAndPlatform = $configPlatformName.Split('|')
    Set-Var -Name "Configuration" -Value $configAndPlatform[0]
    Set-Var -Name "Platform"      -Value $configAndPlatform[1]
}

function HandleChooseNode([System.Xml.XmlNode] $aChooseNode)
{
    SanitizeProjectNode $aChooseNode
    if ($aChooseNode.ChildNodes.Count -eq 0)
    {
        return
    }

    [System.Xml.XmlElement] $selectedChild = $aChooseNode.ChildNodes | `
        Where-Object { $_.GetType().Name -eq "XmlElement" } | `
        Select -first 1

    foreach ($selectedGrandchild in $selectedChild.ChildNodes)
    {
        $aChooseNode.ParentNode.AppendChild($selectedGrandchild.Clone()) | Out-Null
    }

    $aChooseNode.ParentNode.RemoveChild($aChooseNode) | Out-Null
}

function SanitizeProjectNode([System.Xml.XmlNode] $node)
{
    if ($node.Name -ieq "#comment")
    {
        return
    }

    [System.Collections.ArrayList] $nodesToRemove = @()

    if ($node.Name -ieq "#text" -and $node.InnerText.Length -gt 0)
    {
        # evaluate node content
        $node.InnerText = [string](Evaluate-MSBuildExpression $node.InnerText)
    }

    if ($node.Name -ieq "Import")
    {
        [string] $relPath = Evaluate-MSBuildExpression $node.GetAttribute("Project")
        if (!$relPath)
        {
            return
        }
        [string[]] $paths = Canonize-Path -base (Get-Location) -child $relPath -ignoreErrors

        foreach ($path in $paths)
        {
            if (![string]::IsNullOrEmpty($path) -and (Test-Path $path))
            {
                Write-Verbose "Property sheet: $path"
                [string] $currentFile = $global:currentMSBuildFile
                SanitizeProjectFile($path)

                $global:currentMSBuildFile = $currentFile
                InitializeMsBuildCurrentFileProperties -filePath $global:currentMSBuildFile
            }
            else
            {
                Write-Verbose "Could not find property sheet $relPath"
            }
        }
    }

    if ( ($node.Name -ieq "ClCompile" -or $node.Name -ieq "ClInclude") -and
        ![string]::IsNullOrEmpty($node.GetAttribute("Include")) )
    {
        [string] $expandedAttr = Evaluate-MSBuildExpression $node.GetAttribute("Include")
        $node.Attributes["Include"].Value = $expandedAttr
    }

    if ($node.Name -ieq "Choose")
    {
        HandleChooseNode $chooseChild
    }

    if ($node.Name -ieq "Otherwise")
    {
        [System.Xml.XmlElement[]] $siblings = $node.ParentNode.ChildNodes | `
            Where-Object { $_.GetType().Name -ieq "XmlElement" -and $_ -ne $node }
        if ($siblings.Count -gt 0)
        {
            # means there's a <When> element that matched
            # <Otherwise> should not be evaluated, we could set unwated properties
            return
        }
    }

    if ($node.Name -ieq "ItemGroup")
    {
        $childNodes = $node.ChildNodes | Where-Object { $_.GetType().Name -ieq "XmlElement" }
        foreach ($child in $childNodes)
        {
            $childEvaluatedValue = Evaluate-MSBuildExpression $child.GetAttribute("Include")
            Add-Project-Item -name $child.Name -value $childEvaluatedValue
        }

        if ($node.GetAttribute("Label") -ieq "ProjectConfigurations")
        {
            [System.Xml.XmlElement] $firstChild = $childNodes | Select-Object -First 1
            Detect-ProjectDefaultConfigPlatform $firstChild.GetAttribute("Include")
        }
    }

    if ($node.ParentNode.Name -ieq "PropertyGroup")
    {
        # set new property value
        [string] $propertyName = $node.Name
        $propertyValue = Evaluate-MSBuildExpression $node.InnerText

        Set-Var -Name $propertyName -Value $propertyValue

        return
    }

    foreach ($child in $node.ChildNodes)
    {
        [bool] $validChild = $true
        if ($child.GetType().Name -ieq "XmlElement")
        {
            if ($child.HasAttribute("Condition"))
            {
                # process node condition
                [string] $nodeCondition = $child.GetAttribute("Condition")
                $validChild = ((Evaluate-MSBuildCondition($nodeCondition)) -eq $true)
                if ($validChild)
                {
                    $child.RemoveAttribute("Condition")
                }
            }
        }
        if (!$validChild)
        {
            $nodesToRemove.Add($child) | out-null
            continue
        }
        else
        {
            SanitizeProjectNode($child)
        }
    }

    foreach ($nodeToRemove in $nodesToRemove)
    {
        $nodeToRemove.ParentNode.RemoveChild($nodeToRemove) | out-null
    }
}

<#
.DESCRIPTION
   Sanitizes a project xml file, by removing config-platform pairs different from the
   one we selected.
   This is needed so that our XPath selectors don't get confused when looking for data.
#>
function SanitizeProjectFile([string] $projectFilePath)
{
    Write-Verbose "`nSanitizing $projectFilePath"

    [xml] $fileXml = Get-Content $projectFilePath
    $global:projectFiles += @($fileXml)
    $global:xpathNS = New-Object System.Xml.XmlNamespaceManager($fileXml.NameTable)
    $global:xpathNS.AddNamespace("ns", $fileXml.DocumentElement.NamespaceURI)
    $global:currentMSBuildFile = $projectFilePath

    Push-Location (Get-FileDirectory -filePath $projectFilePath)

    InitializeMsBuildCurrentFileProperties -filePath $projectFilePath
    SanitizeProjectNode($fileXml.Project)

    Pop-Location
}

<#
.DESCRIPTION
Loads vcxproj and property sheets into memory. This needs to be called only once
when processing a project. Accessing project nodes can be done using Select-ProjectNodes.
#>
function LoadProject([string] $vcxprojPath)
{
    # Clean global variables that have been set by a previous project load
    Clear-Vars

    $global:vcxprojPath = $vcxprojPath

    InitializeMsBuildProjectProperties

    $global:projectFiles = @()

    SanitizeProjectFile -projectFilePath $global:vcxprojPath

    if ($env:CPT_LOAD_ALL -ne "1")
    {
        # Tries to find a Directory.Build.props property sheet, starting from the
        # project directory, going up. When one is found, the search stops.
        # Multiple Directory.Build.props sheets are not supported.
        [string] $directoryBuildSheetPath = (GetDirNameOfFileAbove -startDir $ProjectDir `
                                             -targetFile "Directory.Build.props") + "\Directory.Build.props"
        if (Test-Path $directoryBuildSheetPath)
        {
            SanitizeProjectFile($directoryBuildSheetPath)
        }

        [string] $vcpkgIncludePath = "$env:LOCALAPPDATA\vcpkg\vcpkg.user.targets"
        if (Test-Path $vcpkgIncludePath)
        {
            SanitizeProjectFile($vcpkgIncludePath)
        }
    }
}

  #-------------------------------------------------------------------------------------------------
# PlatformToolset constants

Set-Variable -name kDefinesUnicode   -value @("-DUNICODE"
,"-D_UNICODE"
) `
-option Constant

Set-Variable -name kDefinesMultiThreaded -value @("-D_MT") `
-option Constant

Set-Variable -name kDefinesClangXpTargeting `
-value @("-D_USING_V110_SDK71_") `
-option Constant

Set-Variable -name kIncludePathsXPTargetingSDK  `
-value "${Env:ProgramFiles(x86)}\Microsoft SDKs\Windows\v7.1A\Include"  `
-option Constant

Set-Variable -name kVStudioDefaultPlatformToolset -Value "v141" -option Constant

Set-Variable -name kClangFlag32BitPlatform        -value "-m32" -option Constant

# ------------------------------------------------------------------------------------------------
# Xpath selectors

Set-Variable -name kVcxprojXpathPreprocessorDefs  `
-value "ns:Project/ns:ItemDefinitionGroup/ns:ClCompile/ns:PreprocessorDefinitions" `
-option Constant

Set-Variable -name kVcxprojXpathAdditionalIncludes `
-value "ns:Project/ns:ItemDefinitionGroup/ns:ClCompile/ns:AdditionalIncludeDirectories" `
-option Constant

Set-Variable -name kVcxprojXpathRuntimeLibrary `
-value "ns:Project/ns:ItemDefinitionGroup/ns:ClCompile/ns:RuntimeLibrary" `
-option Constant

Set-Variable -name kVcxprojXpathHeaders `
-value "ns:Project/ns:ItemGroup/ns:ClInclude" `
-option Constant

Set-Variable -name kVcxprojXpathCompileFiles `
-value "ns:Project/ns:ItemGroup/ns:ClCompile" `
-option Constant

Set-Variable -name kVcxprojXpathWinPlatformVer `
-value "ns:Project/ns:PropertyGroup/ns:WindowsTargetPlatformVersion" `
-option Constant

Set-Variable -name kVcxprojXpathForceIncludes `
-value "ns:Project/ns:ItemDefinitionGroup/ns:ClCompile/ns:ForcedIncludeFiles" `
-option Constant

Set-Variable -name kVcxprojXpathPCH `
-value "ns:Project/ns:ItemGroup/ns:ClCompile/ns:PrecompiledHeader[text()='Create']" `
-option Constant

Set-Variable -name kVcxprojXpathToolset `
-value "ns:Project/ns:PropertyGroup[@Label='Configuration']/ns:PlatformToolset" `
-option Constant

Set-Variable -name kVcxprojXpathCppStandard `
-value "ns:Project/ns:ItemDefinitionGroup/ns:ClCompile/ns:LanguageStandard" `
-option Constant


Set-Variable -name kVcxprojXpathProjectCompileAs `
-value "ns:Project/ns:ItemDefinitionGroup/ns:ClCompile/ns:CompileAs" `
-option Constant

# ------------------------------------------------------------------------------------------------
# Default platform sdks and standard

Set-Variable -name kVSDefaultWinSDK            -value '8.1'             -option Constant
Set-Variable -name kVSDefaultWinSDK_XP         -value '7.0'             -option Constant
Set-Variable -name kDefaultCppStd              -value "stdcpp14"        -option Constant

# ------------------------------------------------------------------------------------------------
Set-Variable -name kCProjectCompile         -value "CompileAsC" -option Constant

Add-Type -TypeDefinition @"
public enum UsePch
{
Use,
NotUsing,
Create
}
"@

Function Should-CompileProject([Parameter(Mandatory = $true)][string] $vcxprojPath)
{
if ($aVcxprojToCompile -eq $null)
{
return $true
}

foreach ($projMatch in $aVcxprojToCompile)
{
if (IsFileMatchingName -filePath $vcxprojPath -matchName $projMatch)
{
return $true
}
}

return $false
}

Function Should-IgnoreProject([Parameter(Mandatory = $true)][string] $vcxprojPath)
{
if ($aVcxprojToIgnore -eq $null)
{
return $false
}

foreach ($projIgnoreMatch in $aVcxprojToIgnore)
{
if (IsFileMatchingName -filePath $vcxprojPath -matchName $projIgnoreMatch)
{
return $true
}
}

return $false
}

Function Should-CompileFile([Parameter(Mandatory = $false)][System.Xml.XmlNode] $fileNode
, [Parameter(Mandatory = $false)][string] $pchCppName
)
{
if ($fileNode -eq $null)
{
return $false
}

[string] $file = $fileNode.Include

if (($file -eq $null) -or (![string]::IsNullOrEmpty($pchCppName) -and ($file -eq $pchCppName)))
{
return $false
}

[System.Xml.XmlNode] $excluded = $fileNode.SelectSingleNode("ns:ExcludedFromBuild", $global:xpathNS)

if (($excluded -ne $null) -and ($excluded.InnerText -ne $null) -and ($excluded.InnerText -ieq "true"))
{
return $false
}

return $true
}

Function Should-IgnoreFile([Parameter(Mandatory = $true)][string] $file)
{
if ($aCppToIgnore -eq $null)
{
return $false
}

foreach ($projIgnoreMatch in $aCppToIgnore)
{
if (IsFileMatchingName -filePath $file -matchName $projIgnoreMatch)
{
return $true
}
}

return $false
}

Function Get-ProjectFilesToCompile([Parameter(Mandatory = $false)][string] $pchCppName)
{
[System.Xml.XmlElement[]] $projectEntries = Select-ProjectNodes($kVcxprojXpathCompileFiles) | `
Where-Object { Should-CompileFile -fileNode $_ -pchCppName $pchCppName }

[System.Collections.ArrayList] $files = @()
foreach ($entry in $projectEntries)
{
[string[]] $matchedFiles = Canonize-Path -base $ProjectDir -child $entry.GetAttribute("Include")
[UsePch] $usePch = [UsePch]::Use

$nodePch = $entry.SelectSingleNode('ns:PrecompiledHeader', $global:xpathNS)
if ($nodePch -and ![string]::IsNullOrEmpty($nodePch.'#text'))
{
switch ($nodePch.'#text')
{
'NotUsing' { $usePch = [UsePch]::NotUsing }
'Create'   { $usePch = [UsePch]::Create   }
}
}

if ($matchedFiles.Count -gt 0)
{
foreach ($file in $matchedFiles)
{
$files += New-Object PsObject -Prop @{ "File"= $file;
          "Pch" = $usePch; }
}
}
}

if ($files.Count -gt 0)
{
$files = @($files | Where-Object { ! (Should-IgnoreFile -file $_.File) })
}

return $files
}

Function Get-ProjectHeaders()
{
[string[]] $headers = Select-ProjectNodes($kVcxprojXpathHeaders) | ForEach-Object {$_.Include }

[string[]] $headerPaths = @()

foreach ($headerEntry in $headers)
{
[string[]] $paths = Canonize-Path -base $ProjectDir -child $headerEntry -ignoreErrors
if ($paths.Count -gt 0)
{
$headerPaths += $paths
}
}
return $headerPaths
}

Function Is-CProject()
{
[string] $compileAs = (Select-ProjectNodes($kVcxprojXpathProjectCompileAs)).InnerText
return $compileAs -eq $kCProjectCompile
}

Function Get-Project-SDKVer()
{
[string] $sdkVer = (Select-ProjectNodes($kVcxprojXpathWinPlatformVer)).InnerText

If ([string]::IsNullOrEmpty($sdkVer)) { "" } Else { $sdkVer.Trim() }
}

Function Is-Project-MultiThreaded()
{
$propGroup = Select-ProjectNodes($kVcxprojXpathRuntimeLibrary)

$runtimeLibrary = $propGroup.InnerText

return ![string]::IsNullOrEmpty($runtimeLibrary)
}

Function Is-Project-Unicode()
{
$propGroup = Select-ProjectNodes("ns:Project/ns:PropertyGroup[@Label='Configuration']/ns:CharacterSet")
if (! $propGroup)
{
return $false
}
return ($propGroup.InnerText -ieq "Unicode")
}

Function Get-Project-CppStandard()
{
[string] $cachedValueVarName = "ClangPowerTools:CppStd"

[string] $cachedVar = (Get-Variable $cachedValueVarName -ErrorAction SilentlyContinue -ValueOnly)
if (![string]::IsNullOrEmpty($cachedVar))
{
return $cachedVar
}

[string] $cppStd = ""

$cppStdNode = Select-ProjectNodes($kVcxprojXpathCppStandard)
if ($cppStdNode)
{
$cppStd = $cppStdNode.InnerText
}
else
{
$cppStd = $kDefaultCppStd
}

$cppStdMap = @{ 'stdcpplatest' = 'c++1z'
; 'stdcpp14'               = 'c++14'
; 'stdcpp17'               = 'c++17'
}

[string] $cppStdClangValue = $cppStdMap[$cppStd]
Set-Var -name $cachedValueVarName -value $cppStdClangValue

return $cppStdClangValue
}

Function Get-ClangCompileFlags([Parameter(Mandatory = $false)][bool] $isCpp = $true)
{
[string[]] $flags = $aClangCompileFlags
if ($isCpp -and !($flags -match "-std=.*"))
{
[string] $cppStandard = Get-Project-CppStandard

$flags = @("-std=$cppStandard") + $flags
}

if ($Platform -ieq "x86" -or $Platform -ieq "Win32")
{
$flags += @($kClangFlag32BitPlatform)
}

return $flags
}

Function Get-ProjectPlatformToolset()
{
$propGroup = Select-ProjectNodes($kVcxprojXpathToolset)

$toolset = $propGroup.InnerText

if ($toolset)
{
return $toolset
}
else
{
return $kVStudioDefaultPlatformToolset
}
}

Function Get-ProjectIncludeDirectories()
{
[string[]] $returnArray = ($IncludePath -split ";")                                   | `
Where-Object { ![string]::IsNullOrWhiteSpace($_) }                                | `
ForEach-Object { Canonize-Path -base $ProjectDir -child $_.Trim() -ignoreErrors } | `
Where-Object { ![string]::IsNullOrEmpty($_) }                                     | `
ForEach-Object { $_ -replace '\\$', '' }
if ($env:CPT_LOAD_ALL -eq '1')
{
return $returnArray
}

[string] $vsPath = Get-VisualStudio-Path
Write-Verbose "Visual Studio location: $vsPath"

[string] $platformToolset = Get-ProjectPlatformToolset

if ($global:cptVisualStudioVersion -eq "2015")
{
$returnArray += Get-VisualStudio-Includes -vsPath $vsPath
}
else
{
$mscVer = Get-MscVer -visualStudioPath $vsPath
Write-Verbose "MSCVER: $mscVer"

$returnArray += Get-VisualStudio-Includes -vsPath $vsPath -mscVer $mscVer
}

$sdkVer = Get-Project-SDKVer

# We did not find a WinSDK version in the vcxproj. We use Visual Studio's defaults
if ([string]::IsNullOrEmpty($sdkVer))
{
if ($platformToolset.EndsWith("xp"))
{
$sdkVer = $kVSDefaultWinSDK_XP
}
else
{
$sdkVer = $kVSDefaultWinSDK
}
}

Write-Verbose "WinSDK version: $sdkVer"

# ----------------------------------------------------------------------------------------------
# Windows 10

if ((![string]::IsNullOrEmpty($sdkVer)) -and ($sdkVer.StartsWith("10")))
{
$returnArray += @("${Env:ProgramFiles(x86)}\Windows Kits\10\Include\$sdkVer\ucrt")

if ($platformToolset.EndsWith("xp"))
{
$returnArray += @($kIncludePathsXPTargetingSDK)
}
else
{
$returnArray += @( "${Env:ProgramFiles(x86)}\Windows Kits\10\Include\$sdkVer\um"
, "${Env:ProgramFiles(x86)}\Windows Kits\10\Include\$sdkVer\shared"
, "${Env:ProgramFiles(x86)}\Windows Kits\10\Include\$sdkVer\winrt"
, "${Env:ProgramFiles(x86)}\Windows Kits\10\Include\$sdkVer\cppwinrt"
)
}
}

# ----------------------------------------------------------------------------------------------
# Windows 8 / 8.1

if ((![string]::IsNullOrEmpty($sdkVer)) -and ($sdkVer.StartsWith("8.")))
{
$returnArray += @("${Env:ProgramFiles(x86)}\Windows Kits\10\Include\10.0.10240.0\ucrt")

if ($platformToolset.EndsWith("xp"))
{
$returnArray += @($kIncludePathsXPTargetingSDK)
}
else
{
$returnArray += @( "${Env:ProgramFiles(x86)}\Windows Kits\$sdkVer\Include\um"
, "${Env:ProgramFiles(x86)}\Windows Kits\$sdkVer\Include\shared"
, "${Env:ProgramFiles(x86)}\Windows Kits\$sdkVer\Include\winrt"
)
}
}

# ----------------------------------------------------------------------------------------------
# Windows 7

if ((![string]::IsNullOrEmpty($sdkVer)) -and ($sdkVer.StartsWith("7.0")))
{
$returnArray += @("$vsPath\VC\Auxiliary\VS\include")

if ($platformToolset.EndsWith("xp"))
{
$returnArray += @( "${Env:ProgramFiles(x86)}\Windows Kits\10\Include\10.0.10240.0\ucrt"
, $kIncludePathsXPTargetingSDK
)
}
else
{
$returnArray += @( "${Env:ProgramFiles(x86)}\Windows Kits\10\Include\7.0\ucrt")
}
}

return ( $returnArray | ForEach-Object { Remove-PathTrailingSlash -path $_ } )
}

<#
.DESCRIPTION
Retrieve directory in which the PCH CPP resides (e.g. stdafx.cpp, stdafxA.cpp)
#>
Function Get-Project-PchCpp()
{
$pchCppRelativePath = Select-ProjectNodes($kVcxprojXpathPCH)   |
Select-Object -ExpandProperty ParentNode |
Select-Object -first 1                   |
Select-Object -ExpandProperty Include

return $pchCppRelativePath
}


<#
.DESCRIPTION
Retrieve array of preprocessor definitions for a given project, in Clang format (-DNAME )
#>
Function Get-ProjectPreprocessorDefines()
{
[string[]] $tokens = (Select-ProjectNodes $kVcxprojXpathPreprocessorDefs).InnerText -split ";"

# make sure we add the required prefix and escape double quotes
[string[]]$defines = ( $tokens | `
ForEach-Object { $_.Trim() } | `
Where-Object { $_ } | `
ForEach-Object { '"' + $(($kClangDefinePrefix + $_) -replace '"', '\"') + '"' } )

if (Is-Project-Unicode)
{
$defines += $kDefinesUnicode
}

if (Is-Project-MultiThreaded)
{
$defines += $kDefinesMultiThreaded
}

[string] $platformToolset = Get-ProjectPlatformToolset
if ($platformToolset.EndsWith("xp"))
{
$defines += $kDefinesClangXpTargeting
}

return $defines
}

Function Get-ProjectAdditionalIncludes()
{
[string[]] $tokens = @()

$data = Select-ProjectNodes $kVcxprojXpathAdditionalIncludes
$tokens += ($data).InnerText -split ";"

foreach ($token in $tokens)
{
if ([string]::IsNullOrWhiteSpace($token))
{
continue
}

[string] $includePath = Canonize-Path -base $ProjectDir -child $token.Trim() -ignoreErrors
if (![string]::IsNullOrEmpty($includePath))
{
$includePath -replace '\\$', ''
}
}
}

Function Get-ProjectForceIncludes()
{
[System.Xml.XmlElement] $forceIncludes = Select-ProjectNodes $kVcxprojXpathForceIncludes
if ($forceIncludes)
{
return $forceIncludes.InnerText -split ";"
}

return $null
}

<#
.DESCRIPTION
Retrieve directory in which stdafx.h resides
#>
Function Get-ProjectStdafxDir( [Parameter(Mandatory = $true)]  [string]   $pchHeaderName
, [Parameter(Mandatory = $false)] [string[]] $includeDirectories
, [Parameter(Mandatory = $false)] [string[]] $additionalIncludeDirectories
)
{
[string] $stdafxPath = ""

[string[]] $projectHeaders = Get-ProjectHeaders
if ($projectHeaders.Count -gt 0)
{
# we need to use only backslashes so that we can match against file header paths
$pchHeaderName = $pchHeaderName.Replace("/", "\")

$stdafxPath = $projectHeaders | Where-Object { (Get-FileName -path $_) -eq $pchHeaderName }
}

if ([string]::IsNullOrEmpty($stdafxPath))
{
[string[]] $searchPool = @($ProjectDir);
if ($includeDirectories.Count -gt 0)
{
$searchPool += $includeDirectories
}
if ($additionalIncludeDirectories.Count -gt 0)
{
$searchPool += $additionalIncludeDirectories
}

foreach ($dir in $searchPool)
{
[string] $stdafxPath = Canonize-Path -base $dir -child $pchHeaderName -ignoreErrors
if (![string]::IsNullOrEmpty($stdafxPath))
{
break
}
}
}

if ([string]::IsNullOrEmpty($stdafxPath))
{
return ""
}
else
{
[string] $stdafxDir = Get-FileDirectory($stdafxPath)
return $stdafxDir
}
}

Function Get-PchCppIncludeHeader([Parameter(Mandatory = $true)][string] $pchCppFile)
{
[string] $cppPath = Canonize-Path -base $ProjectDir -child $pchCppFile

[string[]] $fileLines = Get-Content -path $cppPath
foreach ($line in $fileLines)
{
$regexMatch = [regex]::match($line, '^\s*#include\s+"(\S+)"')
if ($regexMatch.Success)
{
return $regexMatch.Groups[1].Value
}
}
return ""
}

  # line limit for scanning files for #include
[int] $global:cpt_header_include_line_limit = 30

# after the line limit, if any includes are still found we
# extend the limit with this value
[int] $global:cpt_header_include_line_extension = 10

[string[]] $global:headerExtensions = @('h', 'hh', 'hpp', 'hxx')
[string[]] $global:sourceExtensions = @('c', 'cc', 'cpp', 'cxx')

Function detail:FindHeaderReferences( [Parameter(Mandatory = $false)] [string[]] $headers
                                    , [Parameter(Mandatory = $false)] [System.IO.FileInfo[]] $filePool
                                    , [Parameter(Mandatory = $false)] [System.Collections.Hashtable] $alreadyFound = @{}
                                    )
{
    if (!$headers)
    {
        return @()
    }

    [string] $regexHeaders = ($headers | ForEach-Object { ([System.IO.FileInfo]$_).BaseName } `
                                       | Select-Object -Unique `
                                       | Where-Object { $_ -ine "stdafx" -and $_ -ine "resource" } `
                             ) -join '|'

    if ($regexHeaders.Length -eq 0)
    {
        return @()
    }

    [string] $regex = "[/""]($regexHeaders)\.($($global:headerExtensions -join '|'))"""
    Write-Debug "Regex for header reference find: $regex`n"

    [string[]] $returnRefs = @()
    if (!$filePool)
    {
        # initialize pool of files that we look into
        [string[]] $allFileExts = ($global:sourceExtensions + `
                                   $global:headerExtensions) | ForEach-Object { "*.$_" }
        $filePool = Get-ChildItem -recurse -include $allFileExts
    }

    foreach ($file in $filePool)
    {
        if ($alreadyFound.ContainsKey($file.FullName))
        {
            continue
        }

        [int] $lineCount = 0
        [int] $lineLimit = $global:cpt_header_include_line_limit
        foreach($line in [System.IO.File]::ReadLines($file))
        {
            if ([string]::IsNullOrWhiteSpace($line))
            {
                # skip empty lines
                continue
            }

            if ($line -match $regex)
            {
                if ( ! $alreadyFound.ContainsKey($file.FullName))
                {
                    $alreadyFound[$file.FullName] = $true
                    $returnRefs += $file.FullName
                }

                if ($lineCount -eq $lineLimit)
                {
                    # we still have includes to scan
                    $lineLimit += $global:cpt_header_include_line_extension
                }
            }

            if ( (++$lineCount) -gt $lineLimit)
            {
                break
            }
        }
    }

    if ($returnRefs.Count -gt 0)
    {
        [string[]] $headersLeftToSearch = ($returnRefs | Where-Object `
                                          { FileHasExtension -filePath $_ `
                                                             -ext $global:headerExtensions } )
        if ($headersLeftToSearch.Count -gt 0)
        {
            Write-Debug "[!] Recursive reference detection in progress for: "
            Write-Debug ($headersLeftToSearch -join "`n")
            $returnRefs += detail:FindHeaderReferences -headers      $headersLeftToSearch `
                                                       -filePool     $filePool `
                                                       -alreadyFound $alreadyFound
        }
    }

    $returnRefs = $returnRefs | Select-Object -Unique
    Write-Debug "Found header refs (regex $regex)"
    Write-Debug ($returnRefs -join "`n")
    return $returnRefs
}

<#
.SYNOPSIS
Detects source files that reference given headers.

Returns an array with full paths of files that reference the header(s).
.DESCRIPTION
When modifying a header, all translation units that include that header
have to compiled. This function detects those files that include it.
.PARAMETER files
Header files of which we want references to be found
Any files that are not headers will be ignored.
#>
Function Get-HeaderReferences([Parameter(Mandatory = $false)][string[]] $files)
{
    if ($files.Count -eq 0)
    {
        return @()
    }

    # we take interest only in files that reference headers
    $files = $files | Where-Object { FileHasExtension -filePath $_ `
                                                      -ext $global:headerExtensions }

    [string[]] $refs = @()

    if ($files.Count -gt 0)
    {
        Write-Verbose-Timed "Headers changed. Detecting which source files to process..."
        $refs = detail:FindHeaderReferences -headers $files
        Write-Verbose-Timed "Finished detecting source files."
    }

    return $refs
}

<#
.SYNOPSIS
Detects projects that reference given source files (i.e. cpps).

Returns an array with full paths of detected projects.
.DESCRIPTION
When modifying a file, only projects that reference that file should be recompiled.
.PARAMETER projectPool
Projects in which to look
.PARAMETER files
Source files to be found in projects.
#>
Function Get-SourceCodeIncludeProjects([Parameter(Mandatory = $false)][System.IO.FileInfo[]] $projectPool,
                                       [Parameter(Mandatory = $false)][string[]] $files)
{
    [System.Collections.Hashtable] $fileCache = @{}
    foreach ($file in $files)
    {
        if ($file)
        {
            $fileCache[$file.Trim().ToLower()] = $true
        }
    }

    [System.IO.FileInfo[]] $matchedProjects = @()

    [string] $clPrefix    = '<ClCompile Include="'
    [string] $clSuffix    = '" />'
    [string] $endGroupTag = '</ItemGroup>'

    foreach ($proj in $projectPool)
    {
        [string] $projDir  = $proj.Directory.FullName

        [bool] $inClIncludeSection = $false
        foreach($line in [System.IO.File]::ReadLines($proj.FullName))
        {
            $line = $line.Trim()

            if ($line.StartsWith($clPrefix))
            {
                if (!$inClIncludeSection)
                {
                    $inClIncludeSection = $true
                }

                [string] $filePath = $line.Substring($clPrefix.Length, `
                                                     $line.Length - $clPrefix.Length - $clSuffix.Length)
                if (![System.IO.Path]::IsPathRooted($filePath))
                {
                    $filePath = Canonize-Path -base $projDir -child $filePath -ignoreErrors
                }
                if ([string]::IsNullOrEmpty($filePath))
                {
                    continue
                }

                [System.IO.FileInfo] $sourceFile = $filePath
                if ($fileCache.ContainsKey($sourceFile.FullName.Trim().ToLower()) -or `
                    $fileCache.ContainsKey($sourceFile.Name.Trim().ToLower()))
                {
                    $matchedProjects += $proj
                    break
                }
            }

            if ($inClIncludeSection -and $line -eq $endGroupTag)
            {
                # nothing more to check in this project
                break
            }
        }
    }

    return $matchedProjects
}

  
#-------------------------------------------------------------------------------------------------
# Global variables

# temporary files created during project processing (e.g. PCH files)
[System.Collections.ArrayList] $global:FilesToDeleteWhenScriptQuits = @()

# filePath-fileData for SLN files located in source directory
[System.Collections.Generic.Dictionary[String,String]] $global:slnFiles = @{}

# flag to signal when errors are encounteres during project processing
[Boolean]                      $global:FoundErrors                  = $false

# default ClangPowerTools version of visual studio to use
[string] $global:cptDefaultVisualStudioVersion = "2017"

# version of VS currently used
[string] $global:cptVisualStudioVersion = $aVisualStudioVersion

#-------------------------------------------------------------------------------------------------
# Global functions

Function Exit-Script([Parameter(Mandatory=$false)][int] $code = 0)
{
  Write-Verbose-Array -array $global:FilesToDeleteWhenScriptQuits `
                      -name "Cleaning up PCH temporaries"
  # Clean-up
  foreach ($file in $global:FilesToDeleteWhenScriptQuits)
  {
    Remove-Item $file -ErrorAction SilentlyContinue | Out-Null
  }

  # Restore working directory
  Pop-Location

  exit $code
}

Function Fail-Script([parameter(Mandatory=$false)][string] $msg = "Got errors.")
{
  if (![string]::IsNullOrEmpty($msg))
  {
    Write-Err $msg
  }
  Exit-Script($kScriptFailsExitCode)
}

Function Get-SourceDirectory()
{
  [bool] $isDirectory = ($(Get-Item $aSolutionsPath) -is [System.IO.DirectoryInfo])
  if ($isDirectory)
  {
    return $aSolutionsPath
  }
  else
  {
    return (Get-FileDirectory -filePath $aSolutionsPath)
  }
}

function Load-Solutions()
{
   Write-Verbose "Scanning for solution files"
   $slns = Get-ChildItem -recurse -LiteralPath "$aSolutionsPath" -Filter "*$kExtensionSolution"
   foreach ($sln in $slns)
   {
     $slnPath = $sln.FullName
     $global:slnFiles[$slnPath] = (Get-Content $slnPath)
   }

   Write-Verbose-Array -array $global:slnFiles.Keys  -name "Solution file paths"
}

function Get-SolutionProjects([Parameter(Mandatory=$true)][string] $slnPath)
{
  [string] $slnDirectory = Get-FileDirectory -file $slnPath
  $matches = [regex]::Matches($global:slnFiles[$slnPath], 'Project\([{}\"A-Z0-9\-]+\) = \".*?\",\s\"(.*?)\"')
  $projectAbsolutePaths = $matches `
    | ForEach-Object { Canonize-Path -base $slnDirectory `
                                     -child $_.Groups[1].Value.Replace('"','') -ignoreErrors } `
    | Where-Object { ! [string]::IsNullOrEmpty($_) -and $_.EndsWith($kExtensionVcxproj) }
  return $projectAbsolutePaths
}

function Get-ProjectSolution()
{
  foreach ($slnPath in $global:slnFiles.Keys)
  {
    [string[]] $solutionProjectPaths = Get-SolutionProjects $slnPath
    if ($solutionProjectPaths -and $solutionProjectPaths -contains $global:vcxprojPath)
    {
      return $slnPath
    }
  }
  return ""
}

Function Get-Projects()
{
  [string[]] $projects = @()

  foreach ($slnPath in $global:slnFiles.Keys)
  {
    [string[]] $solutionProjects = Get-SolutionProjects -slnPath $slnPath
    if ($solutionProjects.Count -gt 0)
    {
      $projects += $solutionProjects
    }
  }

  return ($projects | Select -Unique);
}

Function Get-ClangIncludeDirectories( [Parameter(Mandatory=$false)][string[]] $includeDirectories
                                    , [Parameter(Mandatory=$false)][string[]] $additionalIncludeDirectories
                                    )
{
  [string[]] $returnDirs = @()

  foreach ($includeDir in $includeDirectories)
  {
    $returnDirs += "-isystem""$includeDir"""
  }
  foreach ($includeDir in $additionalIncludeDirectories)
  {
    if ($aTreatAdditionalIncludesAsSystemIncludes)
    {
      $returnDirs += "-isystem""$includeDir"""
    }
    else
    {
      $returnDirs += "-I""$includeDir"""
    }
  }

  return $returnDirs
}

Function Generate-Pch( [Parameter(Mandatory=$true)] [string]   $stdafxDir
                     , [Parameter(Mandatory=$false)][string[]] $includeDirectories
                     , [Parameter(Mandatory=$false)][string[]] $additionalIncludeDirectories
                     , [Parameter(Mandatory=$true)] [string]   $stdafxHeaderName
                     , [Parameter(Mandatory=$false)][string[]] $preprocessorDefinitions)
{
  
}

Function Get-ExeToCall([Parameter(Mandatory=$true)][WorkloadType] $workloadType)
{
  switch ($workloadType)
  {
     "Tidy"     { return $kClangTidy     }
     "TidyFix"  { return $kClangTidy     }
  }
}

Function Get-CompileCallArguments( [Parameter(Mandatory=$false)][string[]] $preprocessorDefinitions
                                 , [Parameter(Mandatory=$false)][string[]] $includeDirectories
                                 , [Parameter(Mandatory=$false)][string[]] $additionalIncludeDirectories
                                 , [Parameter(Mandatory=$false)][string[]] $forceIncludeFiles
                                 , [Parameter(Mandatory=$false)][string]   $pchFilePath
                                 , [Parameter(Mandatory=$true)][string]    $fileToCompile)
{
  [string[]] $projectCompileArgs = @()
  if (! [string]::IsNullOrEmpty($pchFilePath) -and ! $fileToCompile.EndsWith($kExtensionC))
  {
    $projectCompileArgs += @($kClangFlagIncludePch , """$pchFilePath""")
  }

  $isCpp = $true
  $languageFlag = $kClangFlagFileIsCPP
  if ($fileToCompile.EndsWith($kExtensionC))
  {
    $isCpp = $false
    $languageFlag = $kClangFlagFileIsC
  }

  $projectCompileArgs += @( $languageFlag
                          , """$fileToCompile"""
                          , @(Get-ClangCompileFlags -isCpp $isCpp)
                          , $kClangFlagSupressLINK
                          , $preprocessorDefinitions
                          )

  $projectCompileArgs += Get-ClangIncludeDirectories -includeDirectories           $includeDirectories `
                                                     -additionalIncludeDirectories $additionalIncludeDirectories

  if ($forceIncludeFiles)
  {
    $projectCompileArgs += $kClangFlagNoMsInclude;

    foreach ($file in $forceIncludeFiles)
    {
      $projectCompileArgs += "$kClangFlagForceInclude $file"
    }
  }

  return $projectCompileArgs
}

Function Get-TidyCallArguments( [Parameter(Mandatory=$false)][string[]] $preprocessorDefinitions
                              , [Parameter(Mandatory=$false)][string[]] $includeDirectories
                              , [Parameter(Mandatory=$false)][string[]] $additionalIncludeDirectories
                              , [Parameter(Mandatory=$false)][string[]] $forceIncludeFiles
                              , [Parameter(Mandatory=$true)][string]   $fileToTidy
                              , [Parameter(Mandatory=$false)][string]  $pchFilePath
                              , [Parameter(Mandatory=$false)][switch]  $fix)
{
  [string[]] $tidyArgs = @("""$fileToTidy""")
  if ($fix -and $aTidyFixFlags -ne $kClangTidyUseFile)
  {
    $tidyArgs += "$kClangTidyFlagChecks$aTidyFixFlags"
  }
  elseif ($aTidyFlags -ne $kClangTidyUseFile)
  {
    $tidyArgs += "$kClangTidyFlagChecks$aTidyFlags"
  }
  if($aTidyTreatWarningsAsErrors)
  {
    $tidyArgs += "$kClangTidyFlagWAE$aTidyFlags"
  }

  # The header-filter flag enables clang-tidy to run on headers too.
  if (![string]::IsNullOrEmpty($aTidyHeaderFilter))
  {
    if ($aTidyHeaderFilter -eq '_')
    {
      [string] $fileNameMatch = """$(Get-FileName -path $fileToTidy -noext).*"""
      $tidyArgs += "$kClangTidyFlagHeaderFilter$fileNameMatch"
    }
    else
    {
      $tidyArgs += "$kClangTidyFlagHeaderFilter""$aTidyHeaderFilter"""
    }
  }

  if ($fix)
  {
    if (![string]::IsNullOrEmpty($aAfterTidyFixFormatStyle))
    {
      $tidyArgs += "$kClangTidyFormatStyle$aAfterTidyFixFormatStyle"
    }

    $tidyArgs += $kClangTidyFixFlags
  }
  else
  {
    $tidyArgs += $kClangTidyFlags
  }

  $tidyArgs += Get-ClangIncludeDirectories -includeDirectories           $includeDirectories `
                                           -additionalIncludeDirectories $additionalIncludeDirectories

  $isCpp = $true
  $languageFlag = $kClangFlagFileIsCPP
  if ($fileToTidy.EndsWith($kExtensionC))
  {
    $isCpp = $false
    $languageFlag = $kClangFlagFileIsC
  }

  # We reuse flags used for compilation and preprocessor definitions.
  $tidyArgs += @(Get-ClangCompileFlags -isCpp $isCpp)
  $tidyArgs += $preprocessorDefinitions
  $tidyArgs += $languageFlag

  if (! [string]::IsNullOrEmpty($pchFilePath) -and ! $fileToTidy.EndsWith($kExtensionC))
  {
    $tidyArgs += @($kClangFlagIncludePch , """$pchFilePath""")
  }

  if ($forceIncludeFiles)
  {
    $tidyArgs += $kClangFlagNoMsInclude;

    foreach ($file in $forceIncludeFiles)
    {
      $tidyArgs += "$kClangFlagForceInclude $file"
    }
  }

  return $tidyArgs
}

Function Get-ExeCallArguments( [Parameter(Mandatory=$false)][string]       $pchFilePath
                             , [Parameter(Mandatory=$false)][string[]]     $includeDirectories
                             , [Parameter(Mandatory=$false)][string[]]     $additionalIncludeDirectories
                             , [Parameter(Mandatory=$false)][string[]]     $preprocessorDefinitions
                             , [Parameter(Mandatory=$false)][string[]]     $forceIncludeFiles
                             , [Parameter(Mandatory=$true) ][string]       $currentFile
                             , [Parameter(Mandatory=$true) ][WorkloadType] $workloadType)
{
  switch ($workloadType)
  {
    Tidy    { return Get-TidyCallArguments -preprocessorDefinitions       $preprocessorDefinitions `
                                           -includeDirectories            $includeDirectories `
                                           -additionalIncludeDirectories  $additionalIncludeDirectories `
                                           -forceIncludeFiles             $forceIncludeFiles `
                                           -pchFilePath                   $pchFilePath `
                                           -fileToTidy                    $currentFile }
    TidyFix { return Get-TidyCallArguments -preprocessorDefinitions       $preprocessorDefinitions `
                                           -includeDirectories            $includeDirectories `
                                           -additionalIncludeDirectories  $additionalIncludeDirectories `
                                           -forceIncludeFiles             $forceIncludeFiles `
                                           -pchFilePath                   $pchFilePath `
                                           -fileToTidy                    $currentFile `
                                           -fix}
  }
}

Function Process-ProjectResult($compileResult)
{
  if (!$compileResult.Success)
  {
    Write-Err ($compileResult.Output)

    if (!$aContinueOnError)
    {
      # Wait for other workers to finish. They have a lock on the PCH file
      Get-Job -state Running | Wait-Job | Remove-Job | Out-Null
      Fail-Script
    }

    $global:FoundErrors = $true
  }
  else
  {
    if ( $compileResult.Output.Length -gt 0)
    {
      Write-Output $compileResult.Output
    }
  }
}

Function Wait-AndProcessBuildJobs([switch]$any)
{
  $runningJobs = @(Get-Job -state Running)

  if ($any)
  {
    $runningJobs | Wait-Job -Any | Out-Null
  }
  else
  {
    $runningJobs | Wait-Job | Out-Null
  }

  $jobData = Get-Job -State Completed
  foreach ($job in $jobData)
  {
    $buildResult = Receive-Job $job
    Process-ProjectResult -compileResult $buildResult
  }

  Remove-Job -State Completed
}

Function Wait-ForWorkerJobSlot()
{
  # We allow as many background workers as we have logical CPU cores
  $runningJobs = @(Get-Job -State Running)

  if ($runningJobs.Count -ge $kLogicalCoreCount)
  {
    Wait-AndProcessBuildJobs -any
  }
}

Function Run-ClangJobs( [Parameter(Mandatory=$true)] $clangJobs
                      , [Parameter(Mandatory=$true)][WorkloadType] $workloadType
                      )
{
  # Script block (lambda) for logic to be used when running a clang job.
  $jobWorkToBeDone = `
  {
    param( $job )

    Push-Location $job.WorkingDirectory

    [string] $clangConfigFile = [System.IO.Path]::GetTempFileName()

    [string] $clangConfigContent = ""
    if ($job.FilePath -like '*tidy*')
    {
      # We have to separate Clang args from Tidy args
      $splitparams = $job.ArgumentList -split "--"
      $clangConfigContent = $splitparams[1]
      $job.ArgumentList = ($splitparams[0] + " -- --config ""$clangConfigFile""")
    }
    else
    {
      # Tell Clang to take its args from a config file
      $clangConfigContent = $job.ArgumentList
      $job.ArgumentList = "--config ""$clangConfigFile"""
    }

    # escape slashes for file paths
    # make sure escaped double quotes are not messed up
    $clangConfigContent = $clangConfigContent -replace '\\([^"])', '\\$1'

    # save arguments to clang config file
    $clangConfigContent > $clangConfigFile

    # When PowerShell encounters errors, the first one is handled differently from consecutive ones
    # To circumvent this, do not execute the job directly, but execute it via cmd.exe
    # See also https://stackoverflow.com/a/35980675
    $callOutput = cmd /c $job.FilePath $job.ArgumentList.Split(' ') '2>&1' | Out-String

    $callSuccess = $LASTEXITCODE -eq 0

    Remove-Item $clangConfigFile
    Pop-Location

    return New-Object PsObject -Prop @{ "File"    = $job.File;
                                        "Success" = $callSuccess;
                                        "Output"  = $callOutput }
  }

  [int] $jobCount = $clangJobs.Count
  [int] $crtJobCount = $jobCount

  foreach ($job in $clangJobs)
  {
    # Check if we must wait for background jobs to complete
    Wait-ForWorkerJobSlot

    # Inform console what CPP we are processing next
    Write-Output "$($crtJobCount): $($job.File)"

    # Tidy-fix can cause header corruption when run in parallel
    # because multiple workers modify shared headers concurrently. Do not allow.
    if ($aUseParallelCompile -and $workloadType -ne [WorkloadType]::TidyFix)
    {
      Start-Job -ScriptBlock  $jobWorkToBeDone `
                -ArgumentList $job `
                -ErrorAction Continue | Out-Null
    }
    else
    {
      $compileResult = Invoke-Command -ScriptBlock  $jobWorkToBeDone `
                                      -ArgumentList $job
      Process-ProjectResult -compileResult $compileResult
    }

    $crtJobCount -= 1
  }

  Wait-AndProcessBuildJobs
}

Function Process-Project( [Parameter(Mandatory=$true)][string]       $vcxprojPath
                        , [Parameter(Mandatory=$true)][WorkloadType] $workloadType)
{
  #-----------------------------------------------------------------------------------------------
  # Load data
  LoadProject($vcxprojPath)

  #-----------------------------------------------------------------------------------------------
  # DETECT PLATFORM TOOLSET

  [string] $platformToolset = Get-ProjectPlatformToolset
  Write-Verbose "Platform toolset: $platformToolset"

  if ( $platformToolset -match "^v\d+(_xp)?$" )
  {
    if ( ([int]$platformToolset.Remove(0, 1).Replace("_xp", "")) -le 140)
    {
      if ($global:cptVisualStudioVersion -ne '2015')
      {
        # we need to reload everything and use VS2015
        Write-Verbose "Switching to VS2015 because of v140 toolset. Reloading project..."
        $global:cptVisualStudioVersion = "2015"
        LoadProject($vcxprojPath)
      }
    }
    else
    {
      if ($global:cptVisualStudioVersion -ne $global:cptDefaultVisualStudioVersion)
      {
        # we need to reload everything and the default vs version
        Write-Verbose "Switching to default VsVer because of toolset. Reloading project..."
        $global:cptVisualStudioVersion = $global:cptDefaultVisualStudioVersion
        LoadProject($vcxprojPath)
      }
    }
  }

  #-----------------------------------------------------------------------------------------------
  # FIND FORCE INCLUDES

  [string[]] $forceIncludeFiles = Get-ProjectForceIncludes
  Write-Verbose "Force includes: $forceIncludeFiles"

  #-----------------------------------------------------------------------------------------------
  # DETECT PROJECT PREPROCESSOR DEFINITIONS

  [string[]] $preprocessorDefinitions = Get-ProjectPreprocessorDefines
  if ($global:cptVisualStudioVersion -eq "2017")
  {
    # [HACK] pch generation crashes on VS 15.5 because of STL library, known bug.
    # Triggered by addition of line directives to improve std::function debugging.
    # There's a definition that supresses line directives.

    $preprocessorDefinitions += "-D_DEBUG_FUNCTIONAL_MACHINERY"
  }

  Write-Verbose-Array -array $preprocessorDefinitions -name "Preprocessor definitions"

  #-----------------------------------------------------------------------------------------------
  # DETECT PROJECT ADDITIONAL INCLUDE DIRECTORIES AND CONSTRUCT INCLUDE PATHS

  [string[]] $additionalIncludeDirectories = Get-ProjectAdditionalIncludes
  Write-Verbose-Array -array $additionalIncludeDirectories -name "Additional include directories"

  [string[]] $includeDirectories = Get-ProjectIncludeDirectories
  Write-Verbose-Array -array $includeDirectories -name "Include directories"

  #-----------------------------------------------------------------------------------------------
  # LOCATE STDAFX.H DIRECTORY

  [string] $stdafxCpp    = Get-Project-PchCpp
  [string] $stdafxDir    = ""
  [string] $stdafxHeader = ""
  [string] $stdafxHeaderFullPath = ""

  if (![string]::IsNullOrEmpty($stdafxCpp))
  {
    Write-Verbose "PCH cpp name: $stdafxCpp"

    if ($forceIncludeFiles.Count -gt 0)
    {
      $stdafxHeader = $forceIncludeFiles[0]
    }

    if (!$stdafxHeader)
    {
      $stdafxHeader = Get-PchCppIncludeHeader -pchCppFile $stdafxCpp
    }

    if (!$stdafxHeader)
    {
      $pchNode = Select-ProjectNodes "//ns:ClCompile[@Include='$stdafxCpp']/ns:PrecompiledHeaderFile"
      if ($pchNode)
      {
        $stdafxHeader = $pchNode.InnerText
      }
    }

    Write-Verbose "PCH header name: $stdafxHeader"
    $stdafxDir = Get-ProjectStdafxDir -pchHeaderName                $stdafxHeader       `
                                      -includeDirectories           $includeDirectories `
                                      -additionalIncludeDirectories $additionalIncludeDirectories
  }

  if ([string]::IsNullOrEmpty($stdafxDir))
  {
    Write-Verbose ("PCH not enabled for this project!")
  }
  else
  {
    Write-Verbose ("PCH directory: $stdafxDir")

    $includeDirectories = @(Remove-PathTrailingSlash -path $stdafxDir) + $includeDirectories

    $stdafxHeaderFullPath = Canonize-Path -base $stdafxDir -child $stdafxHeader -ignoreErrors
  }

  #-----------------------------------------------------------------------------------------------
  # FIND LIST OF CPPs TO PROCESS

  [System.Collections.Hashtable] $projCpps = @{}
  foreach ($fileToCompileInfo in (Get-ProjectFilesToCompile -pchCppName $stdafxCpp))
  {
    if ($fileToCompileInfo.File)
    {
      $projCpps[$fileToCompileInfo.File] = $fileToCompileInfo
    }
  }

  if ($projCpps.Count -gt 0 -and $aCppToCompile.Count -gt 0)
  {
    [System.Collections.Hashtable] $filteredCpps = @{}
    [bool] $dirtyStdafx = $false
    foreach ($cpp in $aCppToCompile)
    {
      if ($cpp -ieq $stdafxHeaderFullPath)
      {
        # stdafx modified => compile all
        $dirtyStdafx = $true
        break
      }

      if (![string]::IsNullOrEmpty($cpp))
      {
        if ([System.IO.Path]::IsPathRooted($cpp))
        {
          if ($projCpps.ContainsKey($cpp))
          {
            # really fast, use cache
            $filteredCpps[$cpp] = $projCpps[$cpp]
          }
        }
        else
        {
          # take the slow road and check if it matches
          $projCpps.Keys | Where-Object {  IsFileMatchingName -filePath $_ -matchName $cpp } | `
                          ForEach-Object { $filteredCpps[$_] = $true }
        }
      }
    }

    if (!$dirtyStdafx)
    {
      $projCpps = $filteredCpps
    }
    else
    {
      Write-Verbose "PCH header has been targeted as dirty. Building entire project"
    }
  }
  Write-Verbose ("Processing " + $projCpps.Count + " cpps")

  #-----------------------------------------------------------------------------------------------
  # CREATE PCH IF NEED BE, ONLY FOR TWO CPPS OR MORE

  [string] $pchFilePath = ""
  if ($projCpps.Keys.Count -ge 2 -and
      ![string]::IsNullOrEmpty($stdafxDir))
  {
    # COMPILE PCH
    Write-Verbose "Generating PCH..."
    $pchFilePath = Generate-Pch -stdafxDir        $stdafxDir    `
                                -stdafxHeaderName $stdafxHeader `
                                -preprocessorDefinitions $preprocessorDefinitions `
                                -includeDirectories $includeDirectories `
                                -additionalIncludeDirectories $additionalIncludeDirectories
    Write-Verbose "PCH: $pchFilePath"
  }

  #-----------------------------------------------------------------------------------------------
  # PROCESS CPP FILES. CONSTRUCT COMMAND LINE JOBS TO BE INVOKED

  $clangJobs = @()

  foreach ($cpp in $projCpps.Keys)
  {
    [string] $exeToCall = Get-ExeToCall -workloadType $workloadType

    [string] $finalPchPath = $pchFilePath
    if ($projCpps[$cpp].Pch -eq [UsePch]::NotUsing)
    {
      $finalPchPath = ""
      Write-Verbose "`n[PCH] Will ignore precompiled headers for $cpp`n"
    }

    [string] $exeArgs   = Get-ExeCallArguments -workloadType            $workloadType `
                                               -pchFilePath             $finalPchPath `
                                               -preprocessorDefinitions $preprocessorDefinitions `
                                               -forceIncludeFiles       $forceIncludeFiles `
                                               -currentFile             $cpp `
                                               -includeDirectories      $includeDirectories `
                                               -additionalIncludeDirectories $additionalIncludeDirectories

    $newJob = New-Object PsObject -Prop @{ 'FilePath'        = $exeToCall;
                                           'WorkingDirectory'= Get-SourceDirectory;
                                           'ArgumentList'    = $exeArgs;
                                           'File'            = $cpp }
    $clangJobs += $newJob
  }

  #-----------------------------------------------------------------------------------------------
  # PRINT DIAGNOSTICS

  if ($clangJobs.Count -ge 1)
  {
    Write-Verbose "INVOKE: ""$($global:llvmLocation)\$exeToCall"" $($clangJobs[0].ArgumentList)"
  }

  #-----------------------------------------------------------------------------------------------
  # RUN CLANG JOBS

  Run-ClangJobs -clangJobs $clangJobs -workloadType $workloadType
}

#-------------------------------------------------------------------------------------------------
# Script entry point

#-------------------------------------------------------------------------------------------------
# If we didn't get a location to run CPT at, use the current working directory

if (!$aSolutionsPath)
{
  $aSolutionsPath = Get-Location
}

# ------------------------------------------------------------------------------------------------
# Load param values from configuration file (if exists)

Update-ParametersFromConfigFile

#-------------------------------------------------------------------------------------------------
# Print script parameters

Print-InvocationArguments

#-------------------------------------------------------------------------------------------------
# Script entry point

Write-Verbose "CPU logical core count: $kLogicalCoreCount"

Push-Location (Get-SourceDirectory)

# fetch .sln paths and data
Load-Solutions

# This PowerShell process may already have completed jobs. Discard them.
Remove-Job -State Completed

Write-Verbose "Source directory: $(Get-SourceDirectory)"
Write-Verbose "Scanning for project files"

[System.IO.FileInfo[]] $projects = Get-Projects
[int] $initialProjectCount       = $projects.Count
Write-Verbose ("Found $($projects.Count) projects")

# ------------------------------------------------------------------------------------------------
# If we get headers in the -file arg we have to detect CPPs that include that header

if ($aCppToCompile.Count -gt 0)
{
  # We've been given particular files to compile. If headers are among them
  # we'll find all source files that include them and tag them for processing.
  [string[]] $headerRefs = Get-HeaderReferences -files $aCppToCompile
  if ($headerRefs.Count -gt 0)
  {
    Write-Verbose-Array -name "Detected source files" -array $headerRefs

    $aCppToCompile += $headerRefs
  }
}

# ------------------------------------------------------------------------------------------------

[System.IO.FileInfo[]] $projectsToProcess = @()
[System.IO.FileInfo[]] $ignoredProjects   = @()

if (!$aVcxprojToCompile -and !$aVcxprojToIgnore)
{
  $projectsToProcess = $projects # we process all projects
}
else
{
  # some filtering has to be done

  if ($aVcxprojToCompile)
  {
    $projects = $projects | Where-Object { Should-CompileProject -vcxprojPath $_.FullName }
    $projectsToProcess = $projects
  }

  if ($aVcxprojToIgnore)
  {
    $projectsToProcess = $projects | `
                         Where-Object { !(Should-IgnoreProject  -vcxprojPath $_.FullName ) }

    $ignoredProjects = ($projects | Where-Object { $projectsToProcess -notcontains $_ })
  }
}

if ($projectsToProcess.Count -eq 0)
{
  Write-Err "Cannot find given project(s)"
}

if ($aCppToCompile -and $projectsToProcess.Count -gt 1)
{
  # We've been given particular files to compile, we can narrow down
  # the projects to be processed (those that include any of the particular files)

  # For obvious performance reasons, no filtering is done when there's only one project to process.
  [System.IO.FileInfo[]] $projectsThatIncludeFiles = Get-SourceCodeIncludeProjects -projectPool $projectsToProcess `
                                                                                   -files $aCppToCompile
  Write-Verbose-Array -name "Detected projects" -array $projectsThatIncludeFiles

  # some projects include files using wildcards, we won't match anything in them
  # so when matching nothing we don't do filtering at all
  if ($projectsThatIncludeFiles)
  {
    $projectsToProcess = $projectsThatIncludeFiles
  }
}

if ($projectsToProcess.Count -eq $initialProjectCount)
{
  Write-Verbose "PROCESSING ALL PROJECTS"
}
else
{
  Write-Array -name "PROJECTS" -array $projectsToProcess

  if ($ignoredProjects)
  {
    Write-Array -name "IGNORED PROJECTS" -array $ignoredProjects
  }
}

# ------------------------------------------------------------------------------------------------

$projectCounter = $projectsToProcess.Length;
foreach ($project in $projectsToProcess)
{
  [string] $vcxprojPath = $project.FullName;

  if (![string]::IsNullOrEmpty($aTidyFlags))
  {
     $workloadType = [WorkloadType]::Tidy
  }

  if (![string]::IsNullOrEmpty($aTidyFixFlags))
  {
     $workloadType = [WorkloadType]::TidyFix
  }

  Write-Output ("PROJECT$(if ($projectCounter -gt 1) { " #$projectCounter" } else { } ): " + $vcxprojPath)
  Process-Project -vcxprojPath $vcxprojPath -workloadType $workloadType
  Write-Output "" # empty line separator

  $projectCounter -= 1
}

if ($global:FoundErrors)
{
  Fail-Script
}
else
{
  Exit-Script
}