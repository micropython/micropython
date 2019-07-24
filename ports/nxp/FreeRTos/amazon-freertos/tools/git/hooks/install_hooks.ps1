Remove-Item -Recurse -Force ..\..\..\.git\hooks\*
cmd /c mklink ..\..\..\.git\hooks\pre-commit ..\..\tools\git\hooks\src\pre_commit.py
cmd /c mklink ..\..\..\.git\hooks\commit-msg ..\..\tools\git\hooks\src\commit_msg.py
