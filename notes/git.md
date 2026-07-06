## git show
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


## git log
```shell
git log
git log --oneline
git log -n <number>
```

**By File**: To trace the history of changes made only to a specific file  
`git log -- path/to/file`

**By Author**: To isolate contributions from a single team member  
`git log --author="Name"`

With Visual Graph: To see how branches diverge and merge via an ASCII graph representation,
`git log --graph --oneline --all`

With Code Patches: To review the actual code additions and deletions (diff) introduced by each commit, add the patch flag
`git log -p`

