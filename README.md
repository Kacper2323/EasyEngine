![an banner logo for EasyEngine](./imgREADME/BannerV1.png)
<img src="https://upload.wikimedia.org/wikipedia/commons/thumb/1/18/ISO_C%2B%2B_Logo.svg/180px-ISO_C%2B%2B_Logo.svg.png" width="30" height="32">&nbsp;&nbsp;&nbsp;
<img src="https://www.sfml-dev.org/images/logo.png" width="100" height="32">

### EasyEngine is an ongoing project aimed at creating a game engine for fast prototyping of 2D games.

The motivation for this project was my idea to build a videogame, and a subsequent realization that it takes a lot to make even a simple prototype. I decided that a good place to start learning about game development is to build an engine that allows for the creation of prototypes very quickly and test the idea before jumping into professional engines.

The engine is built with C++20 and SFML and uses the ECS design pattern. The core engine features are based on a 2023 lecture series on game programming from the Memorial University by Dave Churchill [(link)](https://www.youtube.com/watch?v=s99UDGdYIUE&list=PL_xRyXins84_Jf-aCh7chj47HR4oZLPwK&pp=iAQB). The only currently supported platform is Windows.

Below is a graph representing the overall architecture of the engine:
![an banner logo for EasyEngine](./imgREADME/EasyEngine.png)

## Milestones
- [x] core engine features
- [x] basic level editor
- [ ] capable editor (for static elements)
- [ ] config file management class
- [ ] particle system
- [ ] better physics
- [ ] in game windowing system
- [ ] more capable editor (dynamic elements, animations etc.)
- [ ] ability to add premade systems to scenes

## TODO TILL NEXT MILESTONE

- [x] edit components in the editor
- [x] edit sprites in the editor
- [x] save/read the level using a config file
- [ ] save/read created sprites using a config file
- [x] control over the view in the editor
- [x] make the UI more structured
- [x] intuitive mouse controls in the editor
- [ ] creating new scenes in the editor

## Plans for the future
- easy to add premade systems (pathfinding for enemies, spawners etc.)
- visual systems (simple lighting etc.)
- some premade user input systems that depend on the game type (platformer, top-down etc.). Also, add the possibility to fine tune them in UI interface.