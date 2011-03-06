A pre-processing pass on the text data for the Radiohead House of Cards video
http://code.google.com/creative/radiohead/

more info at 
http://memo.tv/radiohead_house_of_cards_binary_pre_processed_data

- cropped the excess area outside of the face.
- mapped the x,y coordinates to a 100x100 grid and quantized them - so they fall exactly on the 100x100 grid. More on this later.
- stored only the height (z) and intensity. No longer need to store x,y as its in a grid.
- removed some of the noise (this can do with some improvement).
- mapped the height (z) to 0..255 so it can be stored in 1 byte.

As a result of this, for every frame there is exactly 100x100 dots, and 2 bytes per dot (z & intensity). Making it 20KB per file. So pretty quick to load each frame. In fact the entire 2101 frame animation can be preloaded only taking up 40MB of memory.

The main reason I did this however, was not only to improve load times, but was so that each dot info will be in the same place in memory across different frames. So a particle system or any complex interaction can easily be created from all the dots while playing the animation.

Also, since the data is normalized (100x100 grid for x,y; 0..255 for z) when loading the data the coordinates need to be adjusted. I found scaling the (x, y, z) values by (4.5, 6, 1.5) respectively works quite well.