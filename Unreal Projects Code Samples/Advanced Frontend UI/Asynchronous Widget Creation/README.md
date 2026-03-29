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

### Asynchronous Widget Creation - Process


### Loading 
How I was able to hook up my UI to the level loading system
