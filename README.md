# ScsRenderSteeringHookups

A (TruckersMP) ATS/ETS2 plugin to allow custom steering wheel Local Mods (Local Only Addon Hookups) to work with the original steering wheel hidden.

The plugin patches some code to force render steering wheel addon hookups when the steering wheel is hidden with the `r_steering_wheel` cvar set to 0.

This is the same plugin that was released months ago, now just made open source and with a download without ads.

### How to use

You can download the plugin from [here](https://github.com/dariowouters/ScsRenderSteeringHookups/releases/latest).  
With the game closed place the `ScsRenderSteeringHookups.dll` in `<game_install_location>/bin/win_x64/plugins`  
(if the plugins folder does not exists, you can create one)

Then when you have a steering wheel Local Mod installed that needs the original wheel hidden you can use the `r_steering_wheel 0` command in the in-game console.


> [!NOTE]  
> The `r_steering_wheel 0` command will hide the wheel for all your trucks, so if you want to show the original wheel again you use the `r_steering_wheel 1` command
