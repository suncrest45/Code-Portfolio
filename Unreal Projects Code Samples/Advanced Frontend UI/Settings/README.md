# Settings
## Result
![](https://github.com/suncrest45/Code-Portfolio/blob/8609f4a11e9c1a13a2cf2aed8519a5d2a23f0f91/Unreal%20Projects%20Code%20Samples/Advanced%20Frontend%20UI/Settings/Settings%20Result%20-%201.png)
![](https://github.com/suncrest45/Code-Portfolio/blob/bccc032a1abe5690da85c4ac317623019ee38551/Unreal%20Projects%20Code%20Samples/Advanced%20Frontend%20UI/Settings/Settings%20Result%20-%202.png)
## Process
### The settings menu is comprised of 4 pieces: **FrontendGameUserSettings**, **OptionsDataRegistry**, **ListDataObjects**, **Widget_EntryLists**
### GameUserSettings    - Handles reading and writing settings to an from a config file
### OptionsDataRegistry - The factory that creates and initialises each setting 
### ListDataObject      - The actual data of the setting, this stores its default value, different values the setting has (low, medium, high, ultra), notifies the framework when the setting has been changed (by condition, by user, or being reset), display name, etc
### Widget_EntryList    - The visual representation of each setting
