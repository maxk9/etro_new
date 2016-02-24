#include "melody.h"

//Player frequency=12000
//Player minimum channels count =4
const flash TPlayerStateChange s_melody_events[223] = {
     { 74, 9 },
     { 62, 8 },
     { 59, 10 },
     { 55, 763 },
     { 67, 376 },
     { 69, 369 },
     { 57, 10 },
     { 71, 371 },
     { 72, 376 },
     { 59, 9 },
     { 74, 770 },
     { 67, 755 },
     { 67, 504 },
     { 0, 9 },
     { 0, 242 },
     { 60, 9 },
     { 76, 762 },
     { 72, 379 },
     { 74, 376 },
     { 76, 377 },
     { 78, 362 },
     { 59, 19 },
     { 79, 760 },
     { 67, 753 },
     { 67, 498 },
     { 0, 19 },
     { 0, 240 },
     { 57, 8 },
     { 72, 515 },
     { 0, 241 },
     { 74, 377 },
     { 72, 378 },
     { 71, 376 },
     { 69, 379 },
     { 55, 9 },
     { 71, 754 },
     { 72, 376 },
     { 71, 379 },
     { 69, 377 },
     { 67, 378 },
     { 62, 8 },
     { 66, 755 },
     { 67, 9 },
     { 59, 378 },
     { 69, 376 },
     { 55, 11 },
     { 71, 377 },
     { 67, 370 },
     { 62, 16 },
     { 71, 107 },
     { 72, 145 },
     { 71, 106 },
     { 72, 192 },
     { 71, 187 },
     { 50, 9 },
     { 69, 378 },
     { 60, 376 },
     { 59, 379 },
     { 57, 385 },
     { 74, 10 },
     { 59, 768 },
     { 67, 363 },
     { 69, 377 },
     { 57, 16 },
     { 71, 378 },
     { 72, 363 },
     { 55, 9 },
     { 74, 752 },
     { 59, 10 },
     { 67, 747 },
     { 55, 9 },
     { 67, 752 },
     { 60, 10 },
     { 76, 755 },
     { 72, 377 },
     { 74, 376 },
     { 76, 378 },
     { 78, 395 },
     { 79, 9 },
     { 59, 752 },
     { 67, 10 },
     { 60, 371 },
     { 59, 376 },
     { 57, 9 },
     { 67, 378 },
     { 55, 379 },
     { 72, 8 },
     { 57, 761 },
     { 74, 378 },
     { 72, 379 },
     { 54, 8 },
     { 71, 361 },
     { 69, 386 },
     { 71, 11 },
     { 55, 744 },
     { 72, 377 },
     { 71, 386 },
     { 69, 11 },
     { 59, 368 },
     { 67, 385 },
     { 60, 10 },
     { 69, 747 },
     { 62, 8 },
     { 71, 377 },
     { 69, 386 },
     { 50, 11 },
     { 67, 384 },
     { 66, 377 },
     { 55, 10 },
     { 67, 1267 },
     { 0, 248 },
     { 43, 128 },
     { 0, 377 },
     { 0, 10 },
     { 0, 235 },
     { 83, 9 },
     { 55, 754 },
     { 79, 371 },
     { 81, 376 },
     { 83, 377 },
     { 79, 394 },
     { 54, 11 },
     { 81, 744 },
     { 74, 369 },
     { 76, 378 },
     { 78, 387 },
     { 74, 384 },
     { 79, 9 },
     { 52, 754 },
     { 55, 11 },
     { 76, 376 },
     { 78, 369 },
     { 52, 10 },
     { 79, 371 },
     { 74, 376 },
     { 57, 9 },
     { 73, 762 },
     { 71, 387 },
     { 73, 376 },
     { 69, 9 },
     { 45, 754 },
     { 57, 11 },
     { 69, 376 },
     { 71, 369 },
     { 73, 378 },
     { 74, 387 },
     { 76, 376 },
     { 78, 377 },
     { 59, 10 },
     { 79, 755 },
     { 62, 8 },
     { 78, 761 },
     { 61, 10 },
     { 76, 755 },
     { 62, 8 },
     { 78, 753 },
     { 54, 10 },
     { 69, 755 },
     { 73, 8 },
     { 57, 761 },
     { 74, 10 },
     { 62, 763 },
     { 50, 512 },
     { 0, 233 },
     { 60, 521 },
     { 0, 10 },
     { 0, 235 },
     { 74, 10 },
     { 59, 515 },
     { 0, 232 },
     { 62, 8 },
     { 67, 377 },
     { 66, 386 },
     { 67, 11 },
     { 59, 752 },
     { 76, 9 },
     { 60, 754 },
     { 67, 8 },
     { 64, 363 },
     { 66, 393 },
     { 60, 10 },
     { 67, 752 },
     { 74, 11 },
     { 59, 745 },
     { 72, 10 },
     { 57, 760 },
     { 55, 11 },
     { 71, 737 },
     { 69, 18 },
     { 62, 360 },
     { 67, 379 },
     { 66, 393 },
     { 67, 378 },
     { 69, 752 },
     { 62, 11 },
     { 50, 369 },
     { 64, 378 },
     { 66, 384 },
     { 67, 379 },
     { 69, 377 },
     { 71, 378 },
     { 72, 8 },
     { 52, 755 },
     { 71, 9 },
     { 55, 754 },
     { 54, 8 },
     { 69, 763 },
     { 55, 9 },
     { 71, 378 },
     { 74, 384 },
     { 47, 11 },
     { 67, 745 },
     { 50, 10 },
     { 66, 760 },
     { 67, 11 },
     { 62, 9 },
     { 55, 10 },
     { 59, 760 },
     { 50, 768 },
     { 43, 505 },
     { 0, 10 },
     { 0, 11 },
     { 0, 0 }
  };

const flash TMelody s_melody = {
    s_melody_events
  };
