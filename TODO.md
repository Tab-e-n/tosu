# OVERALL STRUCTURE

```
tosu.exe
resources/
  sprites/
  sounds/
  data/
    [data_folder]/
      # Same structure as the default asset folders for asset overrides
      # filenames will be hardcoded for assets
      sprites/
        sprite_override.ogg
      sounds/
        sound_override.ogg
      song.ogg
  charts/
    [chart_folder]/
      # Store in which data folder the song is,
      # Store in which data folder sprite and sound overrides are,
      # Each separately so they can be in different places if desired.
      # List all charts used charts
      meta.dat
      # LoadDirectoryFilesEx() can filter for just the chart files
      easy.chart
      medium.chart
      hard.chart
      wackadoo.chart
```

# WHEN LOADING LEVEL
`ChangeDirectory()` to the dir
Asset overrides can get loaded with static paths this way.
When done, go back to Application Directory (`GetApplicationDirectory() or ".."`)

# PARTIALLY IMPLEMENTED
Key rebinds
Speed & difficulty
General system for scaling based on window size

# LIST
Main Menu
Selecting charts for play and editing
Playtesting and regular play don't conflict
Ranks
Finish screen
Saving scores
Note hit sound
Playing songs
Options menu
Loading / Saving options
Customizing Note Colors
Play song in editor to compare if chart syncs up
Logos on startup

# POTENCIAL
Better way to measure accuracy?

