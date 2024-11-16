# Counter-Strike 2 ConVars / Commands

> [!NOTE]  
> cs2 folder contains a list of [Counter-Strike 2](https://steamdb.info/app/730/patchnotes/) console commands and variables including hidden/development-only.  

> [!IMPORTANT]  
> -tools folder contains some additional commands only available with the -tools launch option/parameter. Requires the download of the Workshop Tools DLC. Available after using the command:  `install_dlc_workshoptools_cvar 1`

## Version

```
ClientVersion=2000444
ServerVersion=2000444
PatchVersion=1.40.5.1
ProductName=cs2
appID=730
ServerAppID=2347773
SourceRevision=9350726
VersionDate=Nov 15 2024
VersionTime=13:47:20

```
[steam.inf](https://github.com/SuGolYolLom/CS2-Cvars-Cmds/commits/main/steam.inf)

### Changelog

Are you searching for a particular release? [Click here of the list of updates](https://github.com/SuGolYolLom/CS2-Cvars-Cmds/commits/main/cs2/cvarlist.md) and/or [here for the -tools version](https://github.com/SuGolYolLom/CS2-Cvars-Cmds/commits/main/-tools/cvarlist.md)!  
If you want a cvarlist.log version, it is available [here](https://github.com/SuGolYolLom/CS2-Cvars-Cmds/commits/main/cs2/cvarlist.log) for the cs2 version and [here](https://github.com/SuGolYolLom/CS2-Cvars-Cmds/commits/main/-tools/cvarlist.log) for the -tools version


### Credits

saul/cvar-unhide-s2: https://github.com/saul/cvar-unhide-s2  
ArmynC/ArminC-CS2-Cvars: https://github.com/ArmynC/ArminC-CS2-Cvars  
Poggicek/cs2-docs: https://github.com/Poggicek/cs2-docs / https://cs2.poggu.me/dumped-data/convar-list / https://cs2.poggu.me/dumped-data/command-list  
alliedmodders/hl2sdk: https://github.com/alliedmodders/hl2sdk/tree/cs2 / https://github.com/alliedmodders/hl2sdk/blob/78bdcdaf196a616355a617ce1529b5ef1d991d07/public/tier1/convar.h#L132-L182  
neverlosecc/source2sdk: https://github.com/neverlosecc/source2sdk/tree/cs2

## License
[![License: CC0-1.0](https://img.shields.io/badge/License-CC0%201.0-lightgrey.svg)](https://tldrlegal.com/license/creative-commons-cc0-1.0-universal)

#### FCVAR_Flags Table
| Flag | Value | short alias |
| ---- | ----- | ----------- |
|FCVAR_NONE | 0 | | |
|FCVAR_LINKED_CONCOMMAND | (1<<0) | |
|FCVAR_DEVELOPMENTONLY | (1<<1) | devonly |	
|FCVAR_GAMEDLL | (1<<2) | sv |
|FCVAR_CLIENTDLL | (1<<3) | cl |
|FCVAR_HIDDEN | (1<<4) | hidden |
|FCVAR_PROTECTED | (1<<5) | prot |
|FCVAR_SPONLY | (1<<6) | sp |
|FCVAR_ARCHIVE | (1<<7) | a |
|FCVAR_NOTIFY | (1<<8) | nf |
|FCVAR_USERINFO | (1<<9) | user |
|FCVAR_MISSING0 | (1<<10)  | missing0 |
|FCVAR_UNLOGGED | (1<<11) | unlogged |
|FCVAR_MISSING1 | (1<<12) | missing1 |
|FCVAR_REPLICATED | (1<<13) | rep |
|FCVAR_CHEAT | (1<<14) | cheat |
|FCVAR_PER_USER | (1<<15) | per_user |
|FCVAR_DEMO | (1<<16) | demo |
|FCVAR_DONTRECORD | (1<<17) | norecord |
|FCVAR_MISSING2 | (1<<18) | missing2 |
|FCVAR_RELEASE | (1<<19) | release |
|FCVAR_MENUBAR_ITEM | (1<<20) | menubar_item |
|FCVAR_MISSING3 | (1<<21) | missing3 |
|FCVAR_NOT_CONNECTED | (1<<22) | notconnected |
|FCVAR_VCONSOLE_FUZZY_MATCHING | (1<<23) | vconsole_fuzzy_matching |
|FCVAR_SERVER_CAN_EXECUTE | (1<<24)  | server_can_execute |
|FCVAR_CLIENT_CAN_EXECUTE | (1<<25) | client_can_execute |
|FCVAR_SERVER_CANNOT_QUERY | (1<<26) | server_cannot_query |
|FCVAR_VCONSOLE_SET_FOCUS | (1<<27) | vconsole_set_focus | 
|FCVAR_CLIENTCMD_CAN_EXECUTE | (1<<28) | clientcmd_can_execute |
|FCVAR_EXECUTE_PER_TICK | (1<<29) | per_tick | 