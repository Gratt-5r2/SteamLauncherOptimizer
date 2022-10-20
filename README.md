# Steam Launcher Optimizer
## Improvements
**1. Instant launch of mods.**  
The solved problem: To start running the mod, the launcher copies data from the workshop to the Game directory. To copy 1gb of the mod the launcher needed 10 seconds on average. Instead of copy are uses the symbolic and hard links for now. These are created instantly.

**2. Disk overflow protection.**  
The solved problem: After run the game you will have 2 copies of the mod data. So the typical 3gb mod will take 6gb on you PC. But with links the extra disk space is not required.

**3. **  
