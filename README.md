# � Unicode Browser for Unreal Engine

Unreal Engine Editor Plugin for viewing all supported Unicode Characters.

----
> [!TIP]
> Ever find the *perfect* unicode character for your Unreal editor tooling, only to paste it into Unreal and see a bunch of ��� or ￼￼￼ ?
> 
> Those times are now over, friend.
> 
> Shine light unto darkness, find your way with the 🪄 **Unicode Browser Plugin** 🪄

<div align="center">
<img src="https://github.com/user-attachments/assets/2756db67-f446-49aa-94a0-2428e9c34221" alt="Screenshot 1" width="1000px">
<img src="https://github.com/user-attachments/assets/cc084018-7793-44b9-94ea-7b537e4c4804" alt="Screenshot 2" width="320px">
<img src="https://github.com/user-attachments/assets/a7ffc954-644f-4108-ae30-cdc5491adee1" alt="Screenshot 2" width="320px">
</div>

----

# How to Use

Open Window via `Window -> Unicode Browser`

## Features

* Displays a grid of all characters mapped to a named Unicode Block. (See [UnicodeBlockRange.inl](https://github.com/EpicGames/UnrealEngine/blob/585df42eb3a391efd295abd231333df20cddbcf3/Engine/Source/Runtime/SlateCore/Public/Fonts/UnicodeBlockRange.inl) in Editor Source.).
* Filter displayed characters by Block.
* Lists all named Unicode Blocks that have mapped characters.
* Ability to filter blocks to "only symbols"; preselects Blocks known to have useful, supported symbols e.g. Arrows, Box Drawing, Emoji.
* Ability to change font.
* **Double-click a character to copy it to the system clipboard**.
* Click a block name to scroll to that block.

> [!WARNING]
> **Disclaimer:** Plugin is currently in its infancy and as such leaves much to be desired regarding the user-experience, but it's usable, and useful!

Pull Requests, Bug reports or Suggestions are very welcome!

## Supported Engine Version

The plugin was developed for Unreal Engine 5.5.1+, though it should work for all 5.x versions.

## Installation

The best way is to clone this repository as a submodule; that way you can contribute
pull requests and more importantly, easily get latest updates.
 
The project should be placed in your project's `Plugins` folder.

```bash
cd YourProject
git submodule add git@github.com:ntystudio/UnicodeBrowser.git Plugins/UnicodeBrowser
git commit -m "Added UnicodeBrowser Plugin as a submodule."
```

Alternatively you can download a ZIP of this repo and place it in `YourProject/Plugins/`.

## TODO & Known Issues

* Figure out how to successfully read character names from the icu db e.g. ⚔ -> "Crossed Swords". Currently calling [u_charname](https://github.com/unicode-org/icu/blob/f8aa68b0c1c9584633e7a61157185f1a2c275f58/icu4c/source/common/unames.cpp#L1450) gives me a `u_file_access_error`. Names would be particularly useful for characters which don't display correctly. 
* Use the character names as a starting point to getting useful search e.g. [see this example](https://www.compart.com/en/unicode/search?q=cross#characters)
* Block search & filtering (e.g. hide unchecked languages by default)
* Resizable sidebar
* Figure out how to detect �, ￼ et al., so they can be (optionally) filtered out.
* Persist window state
* Improve initial load time
* Your suggestions?

# License

MIT

----

> [!Tip] 
> # NTY Studio Plugins
>
> * [� Unicode Browser](https://github.com/ntystudio/UnicodeBrowser)
> * [Extended State Tree](https://github.com/ntystudio/ExtendedStateTree)
> * [Reference Visualizer](https://github.com/ntystudio/CTRL-reference-visualizer)
