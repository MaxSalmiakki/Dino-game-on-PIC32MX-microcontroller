# Dino game on PIC32MX microcontroller
As a part of the course "Computer hardware engineering" (IS1200), I was assigned to create a project on the PIC32MX microcontroller. I chose to re-make Google Chrome's popular dinosaur game. I decided to make a project which met the conditions required in order to be categorized as Advanced.

The requirement was to make the dinosaur and the cacti move one pixel at a time. Making cacti move a pixel at a time was easy unlike the dinosaur.

The microcontroller's display uses paged memory, and so initially the dinsaur simply hopped from one page to another. I solved the issue using logical shift operators and representing the character sprites using the unsigned long data type.
