Aim Low, Shoot High: Evading Aimbot Detectors by Mimicking User Behavior
--
Current schemes to detect cheating in on-line games often build
on the assumption that the applied cheat takes actions that are
drastically different from normal behavior. For instance, an Aimbot
for a first-person shooter is used by an amateur player to increase
his/her capabilities many times over. Attempts to evade detection
would require to reduce the intended effect such that the advantage
is presumably lowered into insignificance.  
We argue that this is not necessarily the case and demonstrate how
a professional player is able to make use of an adaptive Aimbot that
mimics user behavior to gradually increase performance and thus
evades state-of-the-art detection mechanisms. We show this in a
quantitative and qualitative evaluation with two professional
"Counter-Strike: Global Offensive" players, two open-source
Anti-Cheat systems, and the commercially established combination of
VAC, VACnet, and Overwatch.


# Player Profiles

The following table provides an overview of the properties to build the
player profiles, which we have used to mimick user behavior.

<img src="https://intellisec.de/research/aimbots/overview.svg" width="700">

For details on the specifics of this properties, please consult the
[conference publication](https://intellisec.de/research/aimbots/2020-eurosec.pdf).


# Code
The repository contains the source code of the Aimbot, its configuration 
file as well as an injector needed to make use of the bot.

### Dependencies

- Latest Version of CMake from <https://cmake.org/download/>
- Build Tools for Visual Studio 2017 from <https://visualstudio.microsoft.com/de/downloads/>

**Attention!** This aimbot has been developed for and tested with
`CS:GO 1.36.8`. It hence might not work with recent versions of the game
anymore.

### Install
```
git clone https://github.com/intellisec/aimbot.git
cd aimbot/tubs_aim
cmake -S . -B build
# If an error occurs: Delete all content from the build folder and repeat this step)

cd build
cmake --build . --config Release
```

# Usage Information

Before the first use, move the `tubs` folder from
`aimbot/tubs_aim_config` to disk at `c:/`. Additionally, you need to
copy the built library `tubs.dll` (cf. installation instructions) to 
`aimbot/tubs_aim_injector`.

To start the aimbot, please follow the following steps:

1. Start the application `aimbot/tubs_aim_injector/tubstarget.exe`
2. Click on the start button
3. Start the game
4. Type the following command in the CS:GO console:
   `unbind mouse1; bind / +attack`

With the corresponding keys, you can then run, enable or disable the
displayed modules.

### Configuration

In the directory `c:/tubs`, you find the configuration file named
`config.json` that can be edited with a text editor of your choice.
Edit the desired values and save the file. In case the Aimbot is running
already, please additional reload the configuration.


# Publication

A detailed description of our work is going to be presented at the
13th European Workshop on Systems Security
([EuroSec](https://www.concordia-h2020.eu/eurosec-2020/)) in April 2020.
If you would like to cite our work, please use the reference as provided
below:

```
@InProceedings{WitWre20,
  author =    {Tim Witschel and Christian Wressnegger},
  title =     {Aim Low, Shoot High: Evading Aimbot Detectors by Mimicking User Behavior},
  booktitle = {Proc. of the {ACM} European Workshop on Systems
               Security ({EuroSec})},
  year =      2020,
  month =     april,
  day =       {27.}
}
```

A preprint of the paper is available [here](https://intellisec.de/research/aimbots/2020-eurosec.pdf).