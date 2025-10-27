This code tests out what can be done with a MSX2 to develop a game with MSXgl. It uses sprites multiplexing, covering 12 solid sprites in all situations, plus up to 40 multiplexed sprites. Multiplexed sprites appear solid when their number is up to 20.

The goal is to measure actual frame rate and different target frame rates, testing workloads for each configuration. This information is presented at the top of the screen (total sprites, soli8d sprites, multiplesed sprites, current frame rate/target frame rate).

Controls are as follows:

- UP Key: increase the number of sprites.
- Down key: decrease the number of sprites.
- LEFT key: increases the target frame rate (up to 60fps)
- LEFT key: increases the target frame rate (down to 10fps)
- SPACE key: switchs between a PSG and a MSX Music song (FM, YM2413).

Possible target frame rates:
- 60
- 30
- 25
- 20
- 15
- 10
