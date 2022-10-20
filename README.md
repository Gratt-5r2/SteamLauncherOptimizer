# SteamLauncherOptimizer
## Improvements
1. Instant launch of mods.  
The solved problem: To start running the mod, the launcher copies data from the workshop to the Game directory. To copy 1gb of the mod the launcher needed 10 seconds on average. Instead of copy are uses the symbolic and hard links for now. These are created instantly.

2.  
