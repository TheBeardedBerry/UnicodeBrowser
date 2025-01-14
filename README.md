# Unicode Browser for Unreal Engine

View All Supported Unicode Characters in Unreal Engine Editor.

Ever find the *perfect* unicode character for your Unreal editor tooling, only to paste it into Unreal and get a bunch of ��� or ￼￼￼ ?

Those times are now over friend.

Shine a light into the darkness and find your way with the 🪄 **Unicode Browser Plugin**

![image](https://github.com/user-attachments/assets/ac6a1e00-3b38-4d3a-b607-fa6a8cf62a36)

Open Window via `Window -> Unicode Browser`

## Features

* Displays a grid of all characters mapped to a named Unicode Block. (See [UnicodeBlockRange.inl](https://github.com/EpicGames/UnrealEngine/blob/585df42eb3a391efd295abd231333df20cddbcf3/Engine/Source/Runtime/SlateCore/Public/Fonts/UnicodeBlockRange.inl) in Editor Source.)
* Filter displayed characters by Block.
* Lists all named Unicode Blocks that have mapped characters.
* Ability to filter blocks to "only symbols"; preselects Blocks known to have useful, supported symbols e.g. Arrows, Box Drawing, Emoji
* Ability to change font.
* **Double-click a character to copy it to the system clipboard**

Pull Requests are very welcome!

**Disclaimer:** Plugin is currently in its infancy and as such leaves much to be desired regarding the user-experience, but it's usable, and useful!

## TODO

* Figure out how to successfully read character names from the icu db, currently calling [u_charname](https://github.com/unicode-org/icu/blob/f8aa68b0c1c9584633e7a61157185f1a2c275f58/icu4c/source/common/unames.cpp#L1450) gives me a `u_file_access_error`.
* Use the character names as a starting point to getting useful search like https://www.compart.com/en/unicode/search?q=cross#characters
* Better sidebar filtering (e.g. hide languages by default)
* Resizable sidebar
* Configurable number of columns (currently fixed at 32 columns)
* Figure out how to detect � et al, so they can be (optionally) filtered out.
* Persist window state
* Improve initial load time
* Your suggestions?

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

# License

MIT
