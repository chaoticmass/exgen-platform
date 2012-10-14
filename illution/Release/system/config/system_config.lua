setvar("configuration", "target-fps", "30");

setvar("configuration", "default-thread-priority", "5");
--[[ configuration.default-thread-priority
    this is the default amount of time a thread can run without yeilding
    measured in milliseconds (1000 ms = 1 second)
    
    setting this number higher will improve the speed an app runs, but 
    may cause framerate to drop. Setting it too low will hamper performance
    but will ensure the UI is always smooth and responsive
    
    Default = 5
]]


setvar("configuration", "default-watchdog-count", "1000");
--[[ configuration.default-watchdog-count
    default number of lines a script will run before the watchdog routine
    is called. Set this number too high and an unresponsive process will cause
    other processes to run slowly or become unresponsive.
    
    Set this number too low and scripts will run needlessly slow.
    
    Default = 1000
]]

setvar("database", "hostname", "65.66.245.90");
--[[ database.hostname 
    
]]

