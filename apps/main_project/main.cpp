// apps/main_project/main.cpp
#include <Grapic.h>

#include "config.h"

using namespace grapic;
using namespace std;

void first_init() { set_config(); }

int main(int, char **) {
  winInit(application_name, DIM_W, DIM_H);
  backgroundColor(120, 70, 200);
  color(220, 70, 100);
  winClear();
  rectangleFill(200, 200, 300, 300);
  pressSpace();
  winQuit();
  return 0;
}
