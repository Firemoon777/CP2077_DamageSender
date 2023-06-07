# Cyberpunk 2077 Damage Sender

Detects all damage in game and sends data to external destination throught UDP

Requires `cyber engine tweaks` and `RED4ext`

Tested on Cyberpunk 2077 v1.62 hotfix 1. RED4ext v1.13.1

# Installation

Assuming `$CYBERPUNK_ROOT` is game root directory, for example, `C:\Program Files (x86)\Steam\steamapps\common\Cyberpunk 2077`

Place `damageSender` into `$CYBERPUNK_ROOT\bin\x64\plugins\cyber_engine_tweaks\mods`
Compile `CP2077_Sockets` and place output dll into `$CYBERPUNK_ROOT\red4ext\plugins`