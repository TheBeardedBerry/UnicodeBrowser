# 🪄 Unicode Browser 🪄 for Unreal Engine

Unreal Engine Editor Plugin for viewing all supported Unicode Characters.

----
> [!TIP]
> Ever find the *perfect* unicode character for your Unreal editor tooling, only to paste it into Unreal and see a bunch of ��� or ￼￼￼ ?
> 
> Those times are now over, friend.
> 
> Shine light unto the darkness, find your way with the 🪄 **Unicode Browser Plugin** 🪄

<div align="center">
<img src="https://github.com/user-attachments/assets/ac6a1e00-3b38-4d3a-b607-fa6a8cf62a36" alt="Screenshot 1" width="1000px">
<img src="https://github.com/user-attachments/assets/6e5ccb9e-e4b6-4a8f-87b8-dfd163a55291" alt="Screenshot 2" width="500px">
<img src="https://github.com/user-attachments/assets/048f77a6-9d9a-4471-981e-3102d21099b8" alt="Screenshot 2" width="500px">

</div>

# How to Use

Open Window via `Window -> Unicode Browser`

## Features

* Displays a grid of all characters mapped to a named Unicode Block. (See [UnicodeBlockRange.inl](https://github.com/EpicGames/UnrealEngine/blob/585df42eb3a391efd295abd231333df20cddbcf3/Engine/Source/Runtime/SlateCore/Public/Fonts/UnicodeBlockRange.inl) in Editor Source.)
* Filter displayed characters by Block.
* Lists all named Unicode Blocks that have mapped characters.
* Ability to filter blocks to "only symbols"; preselects Blocks known to have useful, supported symbols e.g. Arrows, Box Drawing, Emoji
* Ability to change font.
* **Double-click a character to copy it to the system clipboard**

Pull Requests are very welcome!

> [!WARNING]
> **Disclaimer:** Plugin is currently in its infancy and as such leaves much to be desired regarding the user-experience, but it's usable, and useful!

## Supported Engine Version

The plugin was developed for Unreal Engine 5.5.1+, though it should work for all 5.X versions.

## Installation

The best way is to clone this repository as a submodule; that way you can contribute
pull requests if you want and more importantly, easily get latest updates.
 
The project should be placed in your project's `Plugins` folder.

```bash
cd YourProject
git submodule add git@github.com:ntystudio/UnicodeBrowser.git Plugins/UnicodeBrowser
git commit -m "Added UnicodeBrowser Plugin as a submodule."
```

Alternatively you can download a ZIP of this repo and place it in `YourProject/Plugins/`.

## TODO & Known Issues

* Figure out how to successfully read character names from the icu db, currently calling [u_charname](https://github.com/unicode-org/icu/blob/f8aa68b0c1c9584633e7a61157185f1a2c275f58/icu4c/source/common/unames.cpp#L1450) gives me a `u_file_access_error`.
* Use the character names as a starting point to getting useful search like https://www.compart.com/en/unicode/search?q=cross#characters
* Better sidebar filtering (e.g. hide languages by default)
* Resizable sidebar
* Configurable number of columns (currently fixed at 32 columns)
* Figure out how to detect � et al, so they can be (optionally) filtered out.
* Persist window state
* Improve initial load time
* Your suggestions?

# License

MIT
