Dim WshShell

Set WshShell = WScript.CreateObject("WScript.Shell")

WshShell.Run "cmd /c "+WScript.Arguments.Item(0)+"\..\..\..\component\soc\realtek\8195a\misc\iar_utility\common\prebuild.bat "+WScript.Arguments.Item(0), 0