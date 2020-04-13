# Dragonboot

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](https://www.gnu.org/licenses/gpl-2.0.html)
<a href="https://discord.gg/ez2HCgK"><img src="https://discordapp.com/api/guilds/488214231540301826/embed.png" alt="Discord Server" /></a>

## What is Dragonboot?

Dragonboot is a lightweight, simple chainloader based off of argon-nx, designed with binary size and the [DragonInjector](https://github.com/dragoninjector-project/DragonInjector-Project) project in mind. Dragonboot is only ##### bytes, fitting within the constraints of the DragonInjector's 65,536 byte flash storage

## How does it work?

Dragonboot will recieve a number indicating which payload to launch from the DragonInjector. By default, the DragonInjector will indicate that Dragonboot should boot the first payload it finds in the "dragonboot" folder on the root of the SD card. If the user enables the multipayload feature, the DragonInjector will tell Dragonboot to search a specified folder for a payload to launch

## Credits

* __devkitPro:__ for the [devkitARM](https://devkitpro.org/) toolchain.
* __naehrwert__ and __st4rk:__ for the original [hekate](https://github.com/nwert/hekate) project.
* __CTCaer:__ for the continuation of the [hekate](https://github.com/CTCaer/hekate) project.
* __Guillem96:__ for the [argon-nx](https://github.com/Guillem96/argon-nx) project.
* __MatinatorX:__ for the [DragonInjector](https://github.com/dragoninjector-project/DragonInjector-Project) project
* __pixel-stuck:__ lead developer of dragonboot
* All the testers who helped iron out bugs!
