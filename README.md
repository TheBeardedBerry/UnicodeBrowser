# Unicode Browser for Unreal Engine

View All Supported Unicode Characters in Unreal Engine Editor.

Ever find the perfect unicode character for your project only to paste it into unreal and get a bunch of � or ￼ ?
Those times are now over, friend. Find your way with the Unicode Browser plugin.

![image](https://github.com/user-attachments/assets/ac6a1e00-3b38-4d3a-b607-fa6a8cf62a36)

Open Window via `Window -> Unicode Browser`

## Features

* Lists all characters mapped to a named Unicode block
* Lists all Unicode Blocks
* Displays grid of every character in each block
* Filter displayed characters by block
* Ability to filter blocks to "only symbols"; preselects blocks known to have useful, supported symbols.
* Ability to change font
* **Double-click a character to copy it to the system clipboard**

Pull Requests are very welcome!

**Disclaimer:** Plugin is currently in its infancy and as such leaves much to be desired regarding the user-experience, but it's usable, and useful!

## Supported Engine Version

The plugin was developed for Unreal Engine 5.5.1+, though it should work for all 5.X versions.

## Installation

The best way is to clone this repository as a submodule; that way you can contribute
pull requests if you want and more importantly, easily get latest updates.
 
The project should be placed in your project's `Plugins` folder.

```bash
cd YourProject
git submodule add git@github.com:ntystudio/UnicodeBrowser.git Plugins/UnicodeBrowser
git submodule init
git submodule update
git commit -m "Added UnicodeBrowser Plugin as a submodule."
```

Alternatively you can download the ZIP of this repo and place it in `YourProject/Plugins/`.

# License

MIT
