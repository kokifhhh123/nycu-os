# git show
```shell
git show <commit-hash>

git show --stat <commit-hash>
```

Isolates the changes to review only one specific file from that entire commit.  
`git show <commit-hash> -- path/to/file`

Compares the commit against its immediate parent to show the exact changes in a standard patch format.  
`git diff <commit-hash>^ <commit-hash>`


git show, press the q key on your keyboard
```
q: Quit and exit back to your normal terminal prompt.
Spacebar: Scroll down by one full page.
Enter / Down Arrow: Scroll down by one single line.
Up Arrow: Scroll up by one single line.
```


# git log
```shell
git log
git log --oneline
git log -n <number>
```

**By File**: To trace the history of changes made only to a specific file  
`git log -- path/to/file`

**By Author**: To isolate contributions from a single team member  
`git log --author="Name"`

With Visual Graph: To see how branches diverge and merge via an ASCII graph representation  
`git log --graph --oneline --all`

With Code Patches: To review the actual code additions and deletions (diff) introduced by each commit, add the patch flag  
`git log -p`


# git push
```shell
git remote add origin <your-github-repo-url>
git branch -M main
git push -u origin main
```


```shell
git push --force-with-lease
```

```
remote:
origin → https://github.com/koki/kernel.git

upstream:
local main → origin/main
```
git push -u origin main means: `git push --set-upstream origin main`


# git permanently delete a file from the entire Git commit history
```shell
sudo apt install git-filter-repo

git filter-repo --path path/to/file --invert-paths 
(path/to/file should be the git file path not system full path)

git push origin --force --all
git push origin --force --tags
```

# git reset
### Undo commit, keep changes staged
```shell
git reset --soft HEAD~1
```
```
commit is removed  
changes go back to staged area
```

### Undo commit, keep changes unstaged
```shell
git reset --mixed HEAD~1
```
```
commit is removed
changes remain in working directory
but are not staged
```

### Undo commit and discard changes completely
```shell
git reset --hard HEAD~1
```
```
commit is removed
changes are deleted
```

### want to go back to a certain commit, not the closest one
```
git log --oneline
```
Keep changes staged  
`git reset --soft 7b2e110`  
Keep changes unstaged  
`git reset --mixed 7b2e110`  
Delete later changes
`git reset --hard 7b2e110`


# git change commit message
## newest:
```shell
git commit --amend
```
push to remote
```
git push --force-with-lease
```
## 往前 3 筆：
```
git rebase -i HEAD~3
```
```
pick a1b2c3 first message
pick d4e5f6 second message
pick 123abc third message
```
```
reword d4e5f6 second message
```
