# Steam Launcher Optimizer
## Installation and launching
Place the d3d11.dll into the Launcher directory (Gothic\Launcher).  
If you have the workshop and the game on different disks (physical or logical), you should start the launcher as Admin.

## Improvements
**1. Instant mod launching.**  
The solved problem: To start running the mod, the launcher copies data from the workshop to the Game directory. To copy 1gb of the mod the launcher needs 10 seconds on average (1min for 6gb).

**2. Disk overflow protection.**  
The solved problem: After run the game you will have 2 copies of the mod data. So the typical 3gb mod will take 6gb on you PC.

**3. Disk wear protection.**  
The solved problem: Any start of the big modification the launcher creates and deletes the gigabytes of data. This accelerates disk wear.

**4. Correct mod launching order.**  
The solved problem: The launcher changes the timestamps of Steam modifications. This breaks mods from other sources. Because priority of those is always lower than Stream.

**5. Logs size limitation.**  
The solved problem: The launcher logs has no limits. After the year of the active use launcher you can get 1gb of logs.  
  
Instead of coping the data, the optimizer uses the hard links or the symbolic links to read the data directly from the workshop.  
The symbolic links uses when the game and the workshop installed on the different disks. **But for that you must start the launcher as Admin!**

