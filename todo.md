# LEVEL FOLDER
```
[Level]/
  meta.dat
  # LoadDirectoryFilesEx() can filter for just the chart files
  easy.chart
  medium.chart
  hard.chart
  wackadoo.chart

  # Same structure as the default asset folders for asset overrides
  # filenames will be hardcoded for assets
  textures/    
    background.png
  sounds/
    song.ogg
```

# WHEN LOADING LEVEL
`ChangeDirectory()` to the levels dir
Custom assets can get loaded with static paths this way.
When done, go back up

# OTHER
Notes - Decide how they should look
Editing placed notes
Playing songs
Note hit sound
Ranks
Accuracy

