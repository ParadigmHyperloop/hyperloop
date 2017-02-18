//
//  pins.c
//  pod
//
//  Created by Eddie Hurtig on 2/18/17.
//
//

#include "pins.h"

bool setup_pin(int no) {
  // I am being incredibly verbose in my order of operations... can just be
  // a single if with some &&
  if (initPin(no) == 0) {
    if (setPinDirection(no, "out") == 0) {
      if (setPinValue(no, 0) == 0) {
        return true;
      }
    }
  }
  
  return false;
}
