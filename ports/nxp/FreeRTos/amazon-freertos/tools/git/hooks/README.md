# Git-Hooks

## Installation

For Windows, run `.\install_hooks.ps1` in powershell

For Linux and Mac, Run `./install_hooks.sh` from commandline

## Dependencies

* git
* git-secrets: https://github.com/awslabs/git-secrets

## The Hooks

### pre-commit

Runs before creating a commit.
Runs quality tests on code (e.g. static analysis)
Outputs results from failed checks and the command to run to repeat the check

### commit-msg

Runs after saving a commit message.
Checks formatting of the commit message.

## Testing

Run ```py.test``` from the ```.../git/hooks``` directory

### Test Dependencies

* ```mock```
* ```pytest```
* ```pytest-cov```
