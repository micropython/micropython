# https://just.systems
# [windows]
set windows-shell := ["pwsh.exe", "-NoLogo", "-Command"]


default:
    echo 'Hello, world!'

[script("pwsh.exe", "-NoLogo", "-Command")]
cancel-actions:
    $repo = "josverl/micropython"
    gh run list --limit 1000 --json databaseId,status,workflowName --repo $repo |
        ConvertFrom-Json |
        Where-Object status -ne "completed" |
        Where-Object workflowName -like "*port*" |
        ForEach-Object -Parallel {
            gh run cancel $_.databaseId --repo $using:repo
        } -ThrottleLimit 10


# Run the esp rfc2217 server
[working-directory: "scratch/esptool"]
esp2217 com="COM27" port="22170":
    python -m esp_rfc2217_server {{com}} -p {{port}} --no-reset



[working-directory: "tools/mpremote"]
pytest:
    pytest -s -v -t rfc2217://192.168.1.18:22170


snapshot-part-7:
    git branch --force mpr/part_7-before-rebase mpr/part_7


# Run snapshot-part-7 before rewriting part_7, then run this recipe afterwards.
rebase-on-7 old_base="mpr/part_7-before-rebase": (_validate-rebase-on-7 old_base)
    just _rebase-branch-on-7 mpr/part_1 {{old_base}}
    just _rebase-branch-on-7 mpr/part_2 {{old_base}}
    just _rebase-branch-on-7 mpr/part_3 {{old_base}}
    just _rebase-branch-on-7 mpr/part_4 {{old_base}}
    just _rebase-branch-on-7 mpr/part_5 {{old_base}}
    just _rebase-branch-on-7 mpr/part_6 {{old_base}}
    just _rebase-branch-on-7 mpr/part_8 {{old_base}}


[private]
_validate-rebase-on-7 old_base:
    git rev-parse --verify {{old_base}}^{commit}
    git rev-parse --verify mpr/part_7^{commit}
    git diff --quiet
    git diff --cached --quiet
    git merge-base --is-ancestor {{old_base}} mpr/part_1 ; if ($LASTEXITCODE -ne 0) { git merge-base --is-ancestor mpr/part_7 mpr/part_1 }
    git merge-base --is-ancestor {{old_base}} mpr/part_2 ; if ($LASTEXITCODE -ne 0) { git merge-base --is-ancestor mpr/part_7 mpr/part_2 }
    git merge-base --is-ancestor {{old_base}} mpr/part_3 ; if ($LASTEXITCODE -ne 0) { git merge-base --is-ancestor mpr/part_7 mpr/part_3 }
    git merge-base --is-ancestor {{old_base}} mpr/part_4 ; if ($LASTEXITCODE -ne 0) { git merge-base --is-ancestor mpr/part_7 mpr/part_4 }
    git merge-base --is-ancestor {{old_base}} mpr/part_5 ; if ($LASTEXITCODE -ne 0) { git merge-base --is-ancestor mpr/part_7 mpr/part_5 }
    git merge-base --is-ancestor {{old_base}} mpr/part_6 ; if ($LASTEXITCODE -ne 0) { git merge-base --is-ancestor mpr/part_7 mpr/part_6 }
    git merge-base --is-ancestor {{old_base}} mpr/part_8 ; if ($LASTEXITCODE -ne 0) { git merge-base --is-ancestor mpr/part_7 mpr/part_8 }


[private]
_rebase-branch-on-7 branch old_base:
    git merge-base --is-ancestor mpr/part_7 {{branch}} ; if ($LASTEXITCODE -eq 0) { Write-Output "{{branch}} is already based on mpr/part_7" } else { git rebase --onto mpr/part_7 {{old_base}} {{branch}} ; if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE } ; git push --force-with-lease origin {{branch}} }
    