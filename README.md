# Ninja-Jump
# Classes Architecture

I've separated Menu logic from Game logic using different classes. I also use some utility classes, like _RWHelper_ for the EEPROM at the moment (will probably have more of these in the future). States are managed using the _StateMachine_ class.

## Menu Logic

The _Menu_ class contains a pointer to a vector of _Option_ class pointers. The Options themselves are allocated outside of the Menu class, initially I used all my Menus and Options as global static variables, but since that ate up all the available memory quick, I switched to using dynamic allocation. I will probably rewrite it in the future so that the Options are allocated at once with the Menus, instead of existing as a separate, static entity. Deallocation is handled on Menu transitions at the moment for its respective options, so I'm halfway there.

Besides this, the _Menu_ class has a bunch of relevant members for drawing stuff on the LCD. There are also two booleans, one for whether this is a _greeting_ menu or not and one for _in-game_ menus. _Greeting_ menu is a general term I've used for menus that are used just for displaying text for a couple seconds and nothing more. They are special for a couple reasons, but mainly because we don't want to have any sort of cursor blink or the user scrolling through the "options" during such menus. _In-game_ menus are helpful for deciding when to do a state transition, otherwise their properties are similar to _greeting_ menus, with the key difference they don't disappear after a fixed period of time.

I've made a small drawing animation for Menu text. This proved to be quite difficult, especially since I had to take into account the possibility of menus with more than two lines of text. I've also avoided updating the entire lcd for a local text change (i.e. changing an option's value). I'm not going to go to deep into details about this, I've wrote some comments in the code that should clarify how I went about doing these things.

The Options were handled using multiple classes, for each type of Option. I've also defined an OptionType enum, to simulate downcasting, since Arduino doesn't allow classic dynamic_cast C++ downcasting. The classes themselves are pretty self-evident at what they do.

## Game Logic

For the Game logic, I've defined, at the moment of writing this doc, two main classes, the _World_ class and the _Player_ class. I'm going to add a bunch of _Structures_ classes, in order to generate more interesting in-game structures, and I will update the documentation afterwards.

For representing the game map, I've used a _FakeMatrix_ class. The class simulates a regular C++ matrix, by using a dynamically alocated byte array, and counting each byte's bit as a column value. I've overloaded the indexing and assignment operators in such a way that using this class is almost identical to using a regular matrix. Since it's overhead is only two bytes (not including the stuff C++ automatically creates for object headers), I'd say it's a pretty good convenience trade-off, instead of manually using a byte array.

The _World_ class handles two things, generating the world and drawing stuff on the Led matrix. Procedural generation is handled by creating a new structure after the last structure created, in such a way that it is both reachable by the player and at a minimum distance from the last structure, determined by the game's difficulty.

The drawing method isn't too great at the moment, it draws the current camera at every call.

_Player_ movement is handled by "scrolling" the map. Scrolling the map means shifting all matrix rows\columns in one direction, and adding an empty row\column at the end. By doing this, we are essentially never changing the player's actual position, but we are mimicking actual movement. The panning camera therefore just always returns the same dimensions, we're basically moving the entire world, instead of moving the player.

Possible bugs: due to the scrolling feature, it might be possible that the player moves too much to the left\right and accidentally erases parts (or the entire) of the last structure. This could be fixable by using extra data for the structures, but I'm not sure if it's worth it to do this. Heap fragmentation could possibly come into play here. It would also be slower, though probably not by a noticeable margin.

## State Transitions

State transitions are done by checking whether we are in an _in_game_ menu or not, and if yes, whether the current state is _PlayingGame_ or not. Upon switching to the _PlayingGame_ state, the _World_ and _Player_ will be dynamically alocated. After the player dies, we'll switch back to the _BrowsingMenus_ state and deallocate the memory for the _World_ and _Player_ objects.

## EEPROM

Since I've used both ints and bytes for my relevant data, I've decided to use the _EEPROM.get()_ and _EEPROM.put()_ methods for loading and writing data. The documentation says _put()_ calls _update()_, so that should be alright. Not much more to be said, the method that handles writing a high score also searches for the high score's place in the top, and inserts it into the list (that's why it's much bigger than the other methods).
