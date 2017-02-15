Dim WshShell

Set WshShell = WScript.CreateObject("WScript.Shell")

WshShell.Run "cmd /c "+WScript.Arguments.Item(1)+"\..\..\..\component\soc\realtek\8195a\misc\iar_utility\common\postbuild.bat "+WScript.Arguments.Item(0)+" "+WScript.Arguments.Item(1), 0