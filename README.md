# Ninja-Jump

# What is Ninja-Jump?
It's an endless 2D platformer, where your goal is to ascend as high as possible on an infinite tower, which gets a more and more complicated structure as you ascend. I've always loved challenging, but fair games and I think I managed to capture just that with this game. No literally impossible pattern will ever spawn, however they get more and more complex as you ascend, and you only have 2 lives, leaving little room for error.

## Why is it called Ninja-Jump?
Honestly, I don't know why I had the idea to make it Japanese-themed. I don't particularly like Japenese stuff that much. I had recently watched a Sekiro speedrun before starting development, so maybe that's where my inspiration subconsciously came from.

## How to play?
The controls are simple enough. Use the joystick to move on the X-axis and press the button to jump. The longer you press, the higher you jump (up to a certain limit, of course). Don't always rush to jump to the next platform! At higher scores, moving platforms will spawn and if you don't time your jumps right, you might not land on them. I highly recommend playing with sound on, not necessarily because of the great music I used, but because the pattern of disappearing platforms is VERY hard to time right without sound. Disappearing platforms are, as the name suggest, platforms that appear and disappear at certain intervals, and you have to time your jumps correctly, so you land on the next one and so on. Right before disappearing, they will play a sound that indicates you should jump.

As you ascend higher, you will earn better and better ranks. Personally, I can't go higher than the Monk rank.

# Classes Architecture

I've separated Menu logic from Game logic using different classes. I also use some utility classes, like _RWHelper_ for the EEPROM at the moment or the _SoundsManager_ class. States are managed using the _StateMachine_ class.

## Menu Logic

The _Menu_ class contains a pointer to an <strike>vector</strike> array of _Option_ class pointers. <strike>The Options themselves are allocated outside of the Menu class, initially I used all my Menus and Options as global static variables, but since that ate up all the available memory quick, I switched to using dynamic allocation. I will probably rewrite it in the future so that the Options are allocated at once with the Menus, instead of existing as a separate, static entity. Deallocation is handled on Menu transitions at the moment for its respective options, so I'm halfway there.</strike> Options are now dynamically allocated when a Menu is created, and freed on its destruction.

Besides this, the _Menu_ class has a bunch of relevant members for drawing stuff on the LCD. There are also two booleans, one for whether this is a _greeting_ menu or not and one for _in-game_ menus. _Greeting_ menu is a general term I've used for menus that are used just for displaying text for a couple seconds and nothing more. They are special for a couple reasons, but mainly because we don't want to have any sort of cursor blink or the user scrolling through the "options" during such menus. _In-game_ menus are helpful for deciding when to do a state transition, otherwise their properties are similar to _greeting_ menus, with the key difference they don't disappear after a fixed period of time.

I've made a small drawing animation for Menu text. This proved to be quite difficult, especially since I had to take into account the possibility of menus with more than two lines of text. I've also avoided updating the entire lcd for a local text change (i.e. changing an option's value). I'm not going to go to deep into details about this, I've wrote some comments in the code that should clarify how I went about doing these things. Also, this limited my LCD drawing possibilities, since using custom characters on the LCD seems to have the effect of dramatically slowing down the animation. I decided to keep the animation.

I've used throughout the code two different terms, "logical position" and "physical position". By logical position, I'm strictly talking about the order of an Option inside the Menu (i.e. first option, second option etc.) or the order of a line in the Menu's text. Physical position refers to the position where the Option's text starts in its corresponding line, and the line _on the LCD screen_ where it should be drawn. Probably not the best choice of terms, but they naturally came to me while I was documenting the code.

The Options were handled using multiple classes, for each type of Option. I've also defined an OptionType enum, to simulate downcasting, since Arduino doesn't allow classic dynamic_cast C++ downcasting. The classes themselves are pretty self-evident at what they do.

## Game Logic

For the Game logic, I've defined two main classes, the _World_ class and the _Player_ class, and a bunch of _Structures_ sub classes, that serve as building blocks of the world.

For representing the game map, I've used a _FakeMatrix_ class. The class simulates a regular C++ matrix, by using a dynamically alocated byte array, and counting each byte's bit as a column value. I've overloaded the indexing and assignment operators in such a way that using this class is almost identical to using a regular matrix. Since it's overhead is only two bytes (not including the stuff C++ automatically creates for object headers), I'd say it's a pretty good convenience trade-off, instead of manually using a byte array.

The _World_ class handles two things, generating the world and drawing stuff on the Led matrix. Procedural generation is handled by creating a new structure after the last structure created, in such a way that it is both reachable by the player and at a minimum distance from the last structure, determined by the game's difficulty.

There are 6 types of possible structures, and 4 substructures of a certain type (making for a total of 9 possible structures). The most basic structures are straight lines and "pointy" lines. You can imagine them as some sort of Japanese temple roofs thing, conceptually. I haven't created special classes for them, since they can be handled just by drawing them on the map and leaving them there. There **is** a potential problem with this, I will talk about it more in the **Bugs** section.

The third structure type are the _Pagodas_. They look like a sort of triangular structure. I've allocated a special class for them, since they are so big, it's easy for them to get out of the bounds of the world map, so I used the class to make sure I draw them back after they come back in focus (if they do).

The fourth structure type are the _Moving platforms_. As the name suggests, they are platforms that move on the X-axis. Sometimes, when you are up on a structure it will seem like the game bugged and it didn't spawn anything above you. However, just wait a few secs and a moving platform will come into the camera view. Similarly, if you are on a moving platform and it seems there's nothing above it, just have a little patience, to reach both of its ends. It might get a little dizzy when you are on a moving platform and there's another moving platform above you, but it shouldn't be a difficult jump.

The fifth structure are _Disappearing platforms_, which actually consist of the 4 substructures I talked about. These are 4 different patterns of platforms that appear and disappear in a rhythmic fashion. You are able to see the next patterm while waiting for your opportunity to jump, however I will attach a picture of all possible patterns to this README, so it's a bit clearer what patterns you may find.

The sixth structure are the _Canons_. There can spawn up to 3 canons at a time, and they each consist of a platform and a small shooting canon 1 line above it. You will have to avoid getting hit by the projectiles, otherwise you lose one life.

The last 4 structures all have classes associated with them. I've used a linked list in the _World_ class to keep track of these objects. They are freed when their coordinates get out of the map's bounds (not the camera's, because it's nice to sometimes fall back to an older structure, if you miss a jump).

The drawing method draws the current camera at every call. Optimizing this would be both difficult and almost pointless, the only time when the whole camera doesn't change is when the player is not moving AND there are no moving structures on the camera, which doesn't happen all that often.

_Player_ movement is handled by "scrolling" the map. Scrolling the map means shifting all matrix rows\columns in one direction, and adding an empty row\column at the end. By doing this, we are essentially never changing the player's actual position, but we are mimicking actual movement. The panning camera therefore just always returns the same dimensions, we're basically moving the entire world, instead of moving the player. If the player falls to their death, they will be respawned on the last platform they stood on, before falling. Note that getting respawned on a disappearing platform is almost certainly a death sentence, since it's very hard to react in time.

## Sounds
Sounds are handled using the SoundsManager class. Both music and sound effects are handled using its methods. I've used two separate buzzers for this, so that I can play music and sound effects at the same time. The class itself is simple enough.

## State Transitions

State transitions are done by checking whether we are in an _in_game_ menu or not, and if yes, whether the current state is _PlayingGame_ or not. Upon switching to the _PlayingGame_ state, the _World_ and _Player_ will be dynamically alocated. After the player dies, we'll switch back to the _BrowsingMenus_ state and deallocate the memory for the _World_ and _Player_ objects.

## EEPROM

Since I've used both ints and bytes for my relevant data, I've decided to use the _EEPROM.get()_ and _EEPROM.put()_ methods for loading and writing data. The documentation says _put()_ calls _update()_, so that should be alright. Not much more to be said, the method that handles writing a high score also searches for the high score's place in the top, and inserts it into the list (that's why it's much bigger than the other methods).

## Bugs
### The Lonely Point
As far as I'm concerned, this is the only bug and I'm not even sure if I haven't accidentally patched it or not (I haven't been able to replicate it for a while). Sometimes, the last structure spawned will be one point or a line with a length of two points, and then the program just stops spawning new platforms. It's not a memory leak, since dying succesfully brings me to the title screen (and I can start a new playthrough afterwards), and it almost certainly isn't caused by one of the _Structure_ classes. Due to the difficulty of even replicating this behaviour, I can't say for certain whether it's completely gone or not.

My theory is that what's really happening is that another line\pointy line structure (the ones that don't have a class associated to them) is being spawned after the Lonely Point. However, the player does some sort of mistake and falls a couple lines back on an older platform. After they climb back, the map was scrolled down a couple lines during the fall and the structure ends up being **deleted**, but since there's no object handling it, there's nothing called to draw it back on the map. At some point I've increased the amount of lines the matrix has and that might've patched it, but again, since I haven't been able to closely examine the bug, I can't say it's for certain.
