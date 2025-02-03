# � Unicode Browser for Unreal Engine

Unreal Engine Editor Plugin for viewing all supported Unicode Characters.

----
> [!TIP]
> Ever find the *perfect* Unicode character for your Unreal editor tooling, only to paste it into Unreal and see a bunch of ��� or ￼￼￼ ?
> 
> Those times are now over, friend.
> 
> Shine light unto darkness, find your way with the 🪄 **Unicode Browser Plugin** 🪄

----
<div align="center">
<img src="https://github.com/user-attachments/assets/17bd8ab6-10c8-46ce-ad19-eef32308053e" alt="Screenshot 1" width="1000px">
<img src="https://github.com/user-attachments/assets/3bbc99cf-5e2a-4cdd-86a9-6e12c9749643" alt="Screenshot 2" width="320px">
<img src="https://github.com/user-attachments/assets/13b90037-aa18-4a27-8dbb-ab5f6033d14a" alt="Screenshot 2" width="320px">
</div>

# How to Use

Open Window via `Window -> Unicode Browser`

## Features

* Displays a grid of all characters mapped to a named Unicode Block. (See [UnicodeBlockRange.inl](https://github.com/EpicGames/UnrealEngine/blob/585df42eb3a391efd295abd231333df20cddbcf3/Engine/Source/Runtime/SlateCore/Public/Fonts/UnicodeBlockRange.inl) in Editor Source.).
* Filter displayed characters by Block.
* Lists all named Unicode Blocks that have mapped characters.
* Shows zoomed-in view of the character, and additional information when hovering over it.
* Character information: measurements, scaling, subfont index, fallback font used, if any.
* Ability to filter blocks to "only symbols"; preselects Blocks known to have useful, supported symbols e.g. Arrows, Box Drawing, Emoji.
* Ability to change font.
* Show/hide missing characters
* **Double-click a character to copy it to the system clipboard**.
* Click a block name to scroll to that block.
* Zoom: Change font size with CTRL + MouseWheel, change column count with CTRL + SHIFT + MouseWheel
* Search: Search in Tags defined in a preset or by characters/letters, supports multiple search terms at once, e.g.: a,b,c,d

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
* Persist window state
* Settings page for adding new presets
* Your suggestions?

# Thanks

* [ben-mkiv](https://github.com/ben-mkiv) For early testing, feedback and PRs [#1](https://github.com/ntystudio/UnicodeBrowser/pull/1), [#2](https://github.com/ntystudio/UnicodeBrowser/pull/2), [#3](https://github.com/ntystudio/UnicodeBrowser/pull/3)

# License

MIT

----

> [!Tip] 
> # NTY Studio Plugins
>
> * [� Unicode Browser](https://github.com/ntystudio/UnicodeBrowser)
> * [Use State Tree](https://github.com/ntystudio/UseStateTree)
> * [Reference Visualizer](https://github.com/ntystudio/CTRL-reference-visualizer)
