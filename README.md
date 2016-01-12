# voodoo_display
All electronic devices send out eletromagnetic waves, so does our monitor and our monitor does it all the time and at very high frequencies, high enough for our short wave AM radio. All we have to do is display the "correct" image on our screen and out monitor will emit the "right" signals.

Example Screen that generates AM signals.
![voodoo_screencast_data](https://cloud.githubusercontent.com/assets/6825447/12274041/0346b332-b98f-11e5-9d0b-4bd5bf31e86b.gif)
## Dependencies
* SDL >= 2.0 <https://wiki.libsdl.org/Installation>

## Install
```
git clone https://github.com/rohit-dua/voodoo_display.git
cd voodoo_display
g++ voodoo_display.cpp -o voodoo_display -lSDL2
```

## Usage
**[MUSIC]** 
* Generate display to play songs
```
./voodoo_display songs/ddlj.notes
```
* Tune into the carrier frequency(default: 27Mhz) with AM modulation and enjoy the classic!

**[DATA]**
* Generate display to transmit data(default: transmits 'ROHIT DUA') on target monitor
```
./voodoo_display
```
* Start receiver on own system
```
cd receiver/
rtl_fm -M am -f 27M -s 48k -g 50 -l 200 | ./decode_text.py
``` 

##Feedback
Open an issue https://github.com/rohit-dua/voodoo_display/issues to report a bug or request a new feature. Other comments and suggestions can be directly emailed to the authors.
