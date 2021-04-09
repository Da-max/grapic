// apps/main_project/main.cpp
// Il cherchera à faire :
// - évoluer des populations dans un environnement
// - permettre de visualiser les évolutions de ces populations
// - tout ça dans un environnement modifiable et fléxible
// J'ai trouvé de la documentation ici :
// - https://en.wikipedia.org/wiki/Computational_science#Methods_and_algorithms

#include <Grapic.h>
#include <iostream>
#include <string.h>

using namespace grapic;
using namespace std;

// CONST VARIABLES
// ===============

int DIM_H = 720;
int DIM_W = 1320;
const int MAX_CHAR = 100;
const int GRID_WIDTH = 10;
const char TYPES[3][MAX_CHAR] = {"herb", "prey", "predator"};
const int LIFE_DURATION[3] = {100, 200, 300};
const int JEUNE_DURATION[3] = {100, 200, 300};
const char BASE_DIR_IMG[MAX_CHAR] = "data/img/main_project/";
const char EXTENSION_IMG[MAX_CHAR] = ".png";

// STRUCTS
// =======

struct IndividualTypeImages
{
  Image male;
  Image female = male;
  Image child_male = male;
  Image child_female = female;
};

/**
 * This struct contains
 * a type of an individual.  
 * The type is define by an int and a name
 */
struct IndividualType
{
  int type;
  char name[MAX_CHAR];
  int life_duration;
  int jeune_duration;
  int nb_entity;
  IndividualTypeImages images;
};

/**
 * This struct represent an individual,
  * if genre is O, it is a man, else it is a femmal (1).
  * If individual_type is 0, it is a grass, if is 1, it is a prey, else it is a predator.
 */
struct Individual
{
  IndividualType individual_type;
  int genre;
  int jeune_duration = 0;
  int life_duration = 0;
};

/**
 * This struct represent the ecosystem.
 */
struct Ecosystem
{
  Individual grid[GRID_WIDTH][GRID_WIDTH];
  IndividualType individual_types[3];
  int dx, dy;
};

/**
 * This struct contains the size 
 * and the name of the window.
 */
struct Window
{
  char name[MAX_CHAR];
  int width_w, width_h;
  Ecosystem ecosystem;
};

// GETTERS
// =======

/**
 * This function return true if
 * the Individual is an prey. 
 */
bool is_prey(Individual individual)
{
  return individual.individual_type.type == 1;
}

/**
 * This function return true
 * if the individual is a predator.
 */
bool is_predator(Individual individual)
{
  return individual.individual_type.type == 2;
}

/**
 * This function return true
 * if the individual is a herb.
 */
bool is_herb(Individual individual)
{
  return individual.individual_type.type == 0;
}

// MAKE FUNCTIONS
// ==============

IndividualTypeImages make_individual_type_images(IndividualType individual_type)
{
  IndividualTypeImages individual_type_images;
  char base_dir_img[MAX_CHAR], img[MAX_CHAR];
  strcpy(base_dir_img, BASE_DIR_IMG);
  strcat(base_dir_img, TYPES[individual_type.type]);
  strcat(base_dir_img, "/");
  strcat(img, base_dir_img);
  strcat(img, "male");
  strcat(img, EXTENSION_IMG);
  individual_type_images.male = image(img);
  if (individual_type.type > 0)
  {
    strcpy(img, base_dir_img);
    strcat(img, "female");
    strcat(img, EXTENSION_IMG);
    individual_type_images.female = image(img);
    strcpy(img, base_dir_img);
    strcat(img, "child_male");
    strcat(img, EXTENSION_IMG);
    individual_type_images.child_male = image(img);
    strcpy(img, base_dir_img);
    strcat(img, "child_female");
    strcat(img, EXTENSION_IMG);
    individual_type_images.child_female = image(img);
  }
  return individual_type_images;
}

/**
 * Function for make an individual type.
 */
IndividualType make_individual_type(int type)
{
  if (type < 0 || type < 3)
  {
    throw "The type of individu is not valide. It should be between 0 and 3.";
  }

  IndividualType individual_type;

  individual_type.type = type;
  strcpy(individual_type.name, TYPES[type]);
  individual_type.images = make_individual_type_images(individual_type);
  individual_type.jeune_duration = JEUNE_DURATION[type];
  individual_type.life_duration = LIFE_DURATION[type];

  return individual_type;
}

/**
 * Function for make an individual.
 */
Individual make_individual(IndividualType individual_type)
{

  Individual individual;
  individual.individual_type = individual_type;
  if (is_prey(individual) || is_predator(individual))
  {
    individual.genre = frand(0, 2);
  }
  return individual;
}

// INIT FUNCTIONS
// ==============

/**
 * Function for init the individual types. 
 */
void init_individual_types(Ecosystem &ecosystem)
{
  for (int i = 0; i < 3; i++)
  {
    ecosystem.individual_types[i] = make_individual_type(i);
  }
}

/**
 * function for in the ecosystem.
 */
void init_ecosystem(Ecosystem &ecosystem, int size_x, int size_y)
{
  if (size_x > GRID_WIDTH || size_y > GRID_WIDTH || size_x < 0 || size_y < 0)
  {
    throw "The size is too large, it maybe between 1 and MAX_WIDTH";
  }
  ecosystem.dx = size_x;
  ecosystem.dy = size_y;
  for (int i = 0; i < size_x; i++)
  {
    for (int j = 0; j < size_y; j++)
    {
      ecosystem.grid[i][j] = make_individual(ecosystem.individual_types[0]);
    }
  }
}

// CONFIG FUNCTIONS
// ================

void config_individual_types(Ecosystem &ecosystem)
{
  bool stop = false;
  int tmp__nb_type = 0, tmp__nb_type_2 = 0;
  cout << "Configuration des types d’individus." << endl
       << "-----------------------------------" << endl;
  cout << "Veuillez choisir le nombre de d’individu de chaque type que vous souhaitez.";
  while (!stop)
  {
    try
    {
      cout << "Nombre de proie : ";
      cin >> tmp__nb_type;
      cout << "Nombre de prédateur : ";
      cin >> tmp__nb_type_2;
      if (tmp__nb_type == 0 || tmp__nb_type_2 == 0 || tmp__nb_type + tmp__nb_type >= ecosystem.dx * ecosystem.dy)
      {
        throw "les nombres rentrés ne sont pas valides, merci de réessayer.";
      }
      else
      {
        stop = true;
        ecosystem.individual_types[0].nb_entity = (ecosystem.dx * ecosystem.dy) - (tmp__nb_type + tmp__nb_type_2);
        ecosystem.individual_types[1].nb_entity = tmp__nb_type;
        ecosystem.individual_types[2].nb_entity = tmp__nb_type_2;
      }
    }
    catch (const exception &e)
    {
      cout << "Attention, " << e.what() << endl;
    }
  }
}

/**
 * Function to ask and set the Window struct. 
 */
Window config_window()
{
  cout << "Configuration de la fenêtre." << endl
       << "============================" << endl;
  Window window;
  cout << "Bienvenue sur mon système proie-prédateur, merci de définir la taille de la fenêtre "
       << "que vous souhaitez afficher." << endl
       << "Largeur : ";
  cin >> window.width_w;
  cout << "Hauteur : ";
  cin >> window.width_h;
  strcpy(window.name, "Système proie prédateur");
  return window;
}

Ecosystem config_ecosystem()
{
  cout << "Configuration de l’écosystème." << endl
       << "==============================" << endl;
  Ecosystem ecosystem;
  cout << "Veuillez choisir la taille de la grille a utilisé.";
  cout << "Taille en x : ";
  cin >> ecosystem.dx;
  cout << "Taille en y : ";
  cin >> ecosystem.dy;

  // We can procedur for config the individual types.
  init_individual_types(ecosystem);
  config_individual_types(ecosystem);
  return ecosystem;
}

int main(int, char **)
{
  Window w = config_window();
  w.ecosystem = config_ecosystem();
  winInit("Ma super application", DIM_W, DIM_H);
  backgroundColor(120, 70, 200);
  color(220, 70, 100);
  winClear();
  rectangleFill(200, 200, 300, 300);
  pressSpace();
  winQuit();
  return 0;
}
