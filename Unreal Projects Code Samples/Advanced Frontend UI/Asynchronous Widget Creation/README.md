# Advanced Frontend UI

### Asynchronous Widget Creation - Result
![Screenshot of a comment on a GitHub issue showing an image, added in the Markdown, of an Octocat smiling and raising a tentacle.](https://github.com/suncrest45/Code-Portfolio/blob/e533720ff18a9e40b986b958cbc6fa5645f680ea/Unreal%20Projects%20Code%20Samples/Advanced%20Frontend%20UI/Asynchronous%20Widget%20Creation/BlueprintUsage.png)
A user-friendly interface that allows widgets to be pushed to the screen from any blueprint in the project with a bonus of easy to customise confirmation of destructive actions or warnings
### Asynchronous Widget Creation - Setup (Widget_PrimaryLayout)
<p>It starts with 4 stacks arranged in order of input prirority: <br>
Frontend (Main Menu) - lowest input priority <br>
GameHUD (Health bar, Ability bar, lock-on widget, etc.) <br>
GameMenu (Character Screen, Inventory, Skill Tree, Pause Menu, etc.) <br>
Modal (Confirmation of Destructive Action, Warnings, Pop-Ups, etc.) - highest input priority<br>
Widgets are pushed on and popped from these different stacks in addition to dictating input priority they also keep track of which widget you came from</p>

### Asynchronous Widget Creation - Process (FrontendDeveloperSettings, FrontendFunctionLibrary, FrontendUISubsystem, AsyncActions)
![Screenshot of a comment on a GitHub issue showing an image, added in the Markdown, of an Octocat smiling and raising a tentacle.](https://github.com/suncrest45/Code-Portfolio/blob/20fb3974dab010aca1090669e249312bc3e6b5bc/Unreal%20Projects%20Code%20Samples/Advanced%20Frontend%20UI/Asynchronous%20Widget%20Creation/SoftWidgetMap.png)
<p>The developer settings save a map of GameplayTags and Soft Widget Pointers to a config file <br>
The FunctionLibrary uses the tags as keys to find the soft widget pointer <br>
The AsynActions serve as an interface to call FrontendUISubsystem to load in the widgets and push them to the screen</p>
