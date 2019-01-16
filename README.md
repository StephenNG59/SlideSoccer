# SmashSoccer

## An PVP Game
![Cover](https://github.com/StephenNG59/SmashSoccer/blob/master/screenshots/2019-01-14_221117.png "Cover")
### Goal
* Put the ball inside the other player's goal

![Screenshot of Playing](https://github.com/StephenNG59/SmashSoccer/blob/master/screenshots/2019-01-14_230146.png "Screenshot of Playing")
### Operations
* Player1:
  * w/s/a/d to accelerate
  * q/e to switch left/right
  * j/k to accelerate angularly
* Player2:
  * i/k/j/l to accelerate
  * u/o to switch left/right
  * m/. to accelerate angularly
* Others:
  * f1/f2/f3/f4 to change camera view
  * f5 to set all objects static
  * Esc to exit

![Screenshot of Goal](https://github.com/StephenNG59/SmashSoccer/blob/master/screenshots/2019-01-14_230016.png "Screenshot of Goal")
### Modes
1. Normal
 * Sunny and shiny
 * Safe to run around
2. Ghost
 * Dark and thrilling
 * Spotlights to trace your kickers
 * Observe the sparks to catch the ball
![Ghost Mode](https://github.com/StephenNG59/SmashSoccer/blob/master/screenshots/2019-01-14_224548.png "Ghost Mode")
3. Ice
 * Slippy and dangerous
 * The wall disappeared, be careful!
![Ice Mode](https://github.com/StephenNG59/SmashSoccer/blob/master/screenshots/2019-01-14_225703.png "Ice Mode")
4. Ghost and Ice
 * Are you sure?

![Fire Effect](https://github.com/StephenNG59/SmashSoccer/blob/master/screenshots/2019-01-14_224458.png "Fire Effect")

### How to Run
1. Run **SmashSoccer/Debug/SmashSoccer.msi** or **SmashSoccer/Debug/setup.exe** to setup on any path you want, you would get a .exe file;
2. Run the .exe file and have fun playing!

### Tips
1. I suggest not moving the mouse in main menu (although you certainly could try it). If you move it mistakenly, you can **press F1** to get the camera back to a not-weird way.
2. If you want to stop the camera movement, **press F5** in main menu.
3. When playing, if some accidents happen (e.g. all players, or the ball, go out of the pitch), you can **press F5** to reset positions of them. Don't use it when your opponent is about to score though :)
4. The normal way to change game modes is in the main menu. But if you want to change it when playing, **press 3/4** could help you.

### Bugs Currently
1. The colors in main menu don't have enough contrast. Will fix it some time.
2. If the kicker (or the ball) closely touches the goal wall and then move to the wall, it will continuously 'collides' with the wall. This is because of some not-correct optimization in collision detection. Will fix it some time.
3. Sometimes in the game if you press Esc to try to get back to menu, the rendering of the pitch will have problems. Reason for this has not been figured out (or not cared yet). You can press F1 to reset the camera position in menu.

## Credits
Joey de Vries and his great tutorial https://learnopengl.com/
