# Dino game on PIC32MX microcontroller


I wanted to re-make Google Chrome's popular dinosaur game. The goal was to make the dinosaur and the cacti move one pixel at a time. Making cacti move a pixel at a time was easy unlike the dinosaur.

The microcontroller's display uses paged memory, and so initially the dinsaur simply hopped from one page to another. I solved the issue using logical shift operators and representing the character sprites using the unsigned long data type.
