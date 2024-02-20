# ARChide
Easily retrieve and gather the games you have hidden in ARC Browser!

**Introduction**

[ARC Browser by LDX Technology](https://arcbrowser.com/) is an exceptional rom collection browser and emulator frontend for Android, but it lacks a direct feature to delete roms. ARChide addresses this limitation by allowing you to efficiently retrieve and gather all your hidden games inside a dedicated folder.


**Use case**

Imagine you've added a complete romset to your sdcard, but some games are not working or unwanted. While ARC Browser allows you to hide them, the roms still occupy space on your sdcard. ARChide simplifies the process of dealing with these hidden roms.


**Compilation**

Simply use [w64devkit by skeeto](https://github.com/skeeto/w64devkit): ```g++ -O3 -std=c++17 -o ARChide.exe ARChide.cpp -lstdc++fs```


**How to**

1. Launch ARC Browser.
2. Hide unwanted games.
3. Export your list of games to CSV: Tools > Database tasks > List games in database > Done > Export to CSV > Do not include any hash
4. Exit ARC Browser.
5. Retrieve CSV file from `Android/data/net.floatingpoint.android.arcturus/data/games.csv` and move it to your rom folder.

6. Plug the sdcard into your computer.
7. Download `ARChide.exe` and `config.txt` and put them in your rom folder.

8. Configure ARChide
   
If it's your first time using ARChide, edit `config.txt` to add system names and their corresponding folder names inside the rom folder, e.g.:
```
Gameboy=GB
Gameboy Advance=GBA
Gameboy Color=GBC
NES=NES
Nintendo 64=N64
```
If you don't know the exact name of the systems, run `ARChide.exe` to identify the missing systems and add them to the configuration file.

9. Run ARChide (as Administrator)

Once configured, run `ARChide.exe` as Administrator, and all hidden games will be moved to the `_delete` folder while maintaining the folder hierarchy, in case you want to revert.
For example, the hidden game `roms/GBA/Crazy Frog Racer (Europe) (En,Fr,De,Nl).zip` will be automatically move to `roms/_hidden/GBA/Crazy Frog Racer (Europe) (En,Fr,De,Nl).zip`.

Now that all hidden games are gathered in the `_delete` folder, you can do whatever you want with them!

10. Put your sdcard back in your device.
11. Launch ARC Browser.
12. Rescan your collection: Tools > Database tasks > Rescan without scrapping > All
13. Answer YES to the question about the Deleted Games.
