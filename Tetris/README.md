# TETRIS

This is a game made in C using Allegro graphics and multimedia library, read more about Allegro here: [liballeg.org](liballeg.org). This was implemented and tested on Linux. Theoretically you can compile the same code for Windows and Mac.

## HOW TO INSTALL ALLEGRO

If you're using Linux, Allegro installation is simple:

    sudo add-apt-repository ppa:allegro/5.2
    sudo apt-get update
    sudo apt-get install liballegro5-dev
    sudo apt install liballegro-acodec5-dev liballegro-audio5-dev \
        liballegro-image5-dev liballegro-dialog5-dev liballegro-ttf5-dev \
        liballegro-physfs5-dev

## HOW TO COMPILE TETRIS

To compile just clone download the files and use the Makefile:

    git clone https://github.com/lucasturci/Games
    cd Tetris
    make

## HOW TO RUN
Just  `make run`.