# OVERALL STRUCTURE

```
tosu.exe
textures/
sounds/
level/
  [level_name]
```

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
Asset overrides can get loaded with static paths this way.
When done, go back to Application Directory (`GetApplicationDirectory() or ".."`)

# PARTIALLY IMPLEMENTED
Key rebinds

# OTHER
Window size options / Full screen
 - SetConfigFlags(FLAG_WINDOW_RESIZABLE);
 - GetScreenWidth()
 - GetScreenHeight()
 - Game will take up the same sized aspect ratio
 - Background is streched to fit screen (keep aspect just cutoff overflowing bits)

+--------+
|        |
|  Game  |
|        |
+--------+

+--+--------+--+
|  |        |  |
|  |  Game  |  |
|  |        |  |
+--+--------+--+

+--------+
+--------+
|        |
|  Game  |
|        |
+--------+
+--------+

Play song in editor to compare if chart syncs up
Playing songs
Note hit sound
Ranks
Customizing Note Colors
Loading / Saving options

# POTENCIAL
Better way to measure accuracy?

