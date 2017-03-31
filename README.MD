# CCF Project
<!-- ### Will & Arthur -->
---
### Use:


#### Build as follows:
* make


#### Run as follows:
* make test
* ./af_interface &

#### Use as follows:

* typing "afarm" anywhere/anytime arms system
* typing "afdisarm" anywhere/anytime disarms system
* typing "afhide" anywhere/anytime will hide kit
* typing "afunhide" anywhere/anytime will reveal kit

---
### Settings:

Settings are compile time, and are set by editing af_common.h

Of particular importance:
* You must set KEYBOARD_INPUT to be the correct device on your machine.  To find the device, look at Xorg log.
* Set the AF technique as follows:
..* 0 --> Nothing (Default)
..* 1 --> Call Scrambler
..* 2 --> System Request shutdown
..* 3 --> Zero cr3