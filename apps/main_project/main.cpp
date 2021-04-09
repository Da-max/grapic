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

const int MAX_CHAR = 1000;
const int GRID_WIDTH = 10;
const char TYPES[3][MAX_CHAR] = {"herb", "prey", "predator"};
const int LIFE_DURATION[3] = {100, 200, 300};
const int JEUNE_DURATION[3] = {100, 200, 300};
const char BASE_DIR_IMG[MAX_CHAR] = "data/img/main_project/";
const char EXTENSION_IMG[MAX_CHAR] = ".png";

// STRUCTS
// =======

struct Position {
    float x, y;
};

struct IndividualTypeImages {
    Image male;
    Image female;
    Image child_male;
    Image child_female;
};

/**
 * This struct contains
 * a type of an individual.
 * The type is define by an int and a name
 */
struct IndividualType {
    int type = 0;
    char name[MAX_CHAR] = "herb";
    int life_duration = 0;
    int jeune_duration = 0;
    int nb_entity = 0;
    IndividualTypeImages images;
};

/**
 * This struct represent an individual,
  * if genre is O, it is a man, else it is a female (1).
  * If individual_type is 0, it is a grass, if is 1, it is a prey, else it is a predator.
 */
struct Individual {
    IndividualType individual_type;
    int genre = -1;
    int jeune_duration = 0;
    int life_duration = 0;
};

/**
 * This struct represent the ecosystem.
 */
struct Ecosystem {
    Individual grid[GRID_WIDTH][GRID_WIDTH];
    IndividualType individual_types[3];
    int dx, dy;
};

/**
 * This struct contains the size 
 * and the name of the window.
 */
struct Window {
    char name[MAX_CHAR];
    int width_w, width_h;
    Ecosystem ecosystem;
};

// GETTERS
// =======

/**
 *
 * This function return true if
 * the Individual is an prey.
 * @param individual the individual analysed
 * @return bool
 */
bool is_prey(Individual individual) {
    return individual.individual_type.type == 1;
}

/**
 * This function return true
 * if the individual is a predator.
 * @param individual the individual analysed
 * @return bool
 */
bool is_predator(Individual individual) {
    return individual.individual_type.type == 2;
}

/**
 * This function return true
 * if the individual is a herb.
 * @param individual the individual analysed
 * @return bool
 */
bool is_herb(Individual individual) {
    return individual.individual_type.type == 0;
}

/**
 * Return true if the individual is male.
 * @param individual The individual analysed.
 * @return bool.
 */
bool is_male(Individual individual) {
    if (individual.genre < 0) {
        throw invalid_argument("L’individu n’a pas de genre, ou celui-ci n’est pas défini.");
    }
    return individual.genre < 1;
}

/**
 * Return true if the individual is female.
 * @param individual The individual analysed.
 * @return bool.
 */
bool is_female(Individual individual) {
    return !is_male(individual);
}

/**
 * Return if the individual is old (if his life_duration > LIFE_DURATION[type] / 2)
 * @param individual The individual analysed.
 * @return bool.
 */
bool is_old(Individual individual) {
    return individual.life_duration > LIFE_DURATION[individual.individual_type.type] / 2;
}

// UTILS FUNCTIONS

float compute_ratio(Window window) {
    return min(window.width_h / window.ecosystem.dy, window.width_w / window.ecosystem.dx);
}

// MAKE FUNCTIONS
// ==============

/**
 * Function for make a position (or Complex).
 * @param x
 * @param y
 * @return Position
 */
Position make_position(float x, float y) {
    Position p;
    p.x = x;
    p.y = y;
    return p;
}

/**
 * This function create an individual type images.
 * @param individual_type the type of individual for whom the images are to be created
 * @return IndividualTypeImages created
 */
IndividualTypeImages make_individual_type_images(int type) {
    IndividualTypeImages individual_type_images;
    char base_dir_img[MAX_CHAR], img[MAX_CHAR];
    strcpy(base_dir_img, BASE_DIR_IMG);
    strcat(base_dir_img, TYPES[type]);
    strcat(base_dir_img, "/");
    strcpy(img, base_dir_img);
    strcat(img, "male");
    strcat(img, EXTENSION_IMG);
    individual_type_images.male = image(img);
    individual_type_images.female = image(img);
    individual_type_images.child_female = image(img);
    individual_type_images.child_male = image(img);
    if (type > 100) {
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
 * Function for create an individual type
 * @param type type
 * @return IndividualType
 */
IndividualType make_individual_type(int type) {
    if (type < 0 || type > 3) {
        throw invalid_argument("The type of individu is not valide. It should be between 0 and 3.");
    }

    IndividualType individual_type;
    IndividualTypeImages individual_type_images;

    individual_type.type = 0;
    strcpy(individual_type.name, TYPES[type]);
    individual_type.jeune_duration = JEUNE_DURATION[type];
    individual_type.life_duration = LIFE_DURATION[type];
    individual_type.images = make_individual_type_images(individual_type.type);

    return individual_type;
}

/**
 * Function for make an individual.
 * @param individual_type IndividualType the type of individual created
 * @return Individual
 */
Individual make_individual(IndividualType individual_type) {
    Individual individual;
    individual.individual_type = individual_type;
    if (is_prey(individual) || is_predator(individual)) {
        individual.genre = frand(0, 2);
    }
    return individual;
}

// INIT FUNCTIONS
// ==============

/**
 * Function for init each individual types.
 * @param ecosystem Ecosystem to which the type of individual belongs
 */
void init_individual_types(Ecosystem &ecosystem) {
    for (int i = 0; i < 3; i++) {
        ecosystem.individual_types[i] = make_individual_type(i);
    }
}

// CONFIG FUNCTIONS

/**
 * function for init the ecosystem.
 * @param ecosystem Ecocsytem to init
 * @param size_x size_x of the ecosystem
 * @param size_y size_y on the ecosystem
 */
void init_ecosystem_grid(Ecosystem &ecosystem) {
    for (int i = 0; i < ecosystem.dx; i++) {
        for (int j = 0; j < ecosystem.dy; j++) {
            ecosystem.grid[i][j] = make_individual(ecosystem.individual_types[0]);
        }
    }
}
// ================

/**
 * Procedure to config an individual_type
 * @param ecosystem Ecosystem of the inidvidual_type
 */
void config_individual_types(Ecosystem &ecosystem) {
    bool stop = false;
    int tmp__nb_type = 0, tmp__nb_type_2 = 0;
    cout << "Configuration des types d’individus." << endl
         << "-----------------------------------" << endl;
    cout << "Veuillez choisir le nombre de d’individu de chaque type que vous souhaitez." << endl;
    while (!stop) {
        try {
            cout << "Nombre de proie : ";
            cin >> tmp__nb_type;
            cout << "Nombre de prédateur : ";
            cin >> tmp__nb_type_2;
            if (tmp__nb_type == 0 || tmp__nb_type_2 == 0 ||
                tmp__nb_type + tmp__nb_type >= ecosystem.dx * ecosystem.dy) {
                throw invalid_argument("Les nombres rentrés ne sont pas valides, merci de réessayer.");
            } else {
                stop = true;
                ecosystem.individual_types[0].nb_entity =
                        (ecosystem.dx * ecosystem.dy) - (tmp__nb_type + tmp__nb_type_2);
                ecosystem.individual_types[1].nb_entity = tmp__nb_type;
                ecosystem.individual_types[2].nb_entity = tmp__nb_type_2;
            }
        }
        catch (const exception &e) {
            cout << "Attention, " << e.what() << endl;
        }
    }
}

/**
 * Function to ask and set the Window struct.
 * @return Window
 */
Window config_window() {
    Window window;
    cout << "Configuration de la fenêtre." << endl
         << "============================" << endl;
    cout << "Bienvenue sur mon système proie-prédateur, merci de définir la taille de la fenêtre "
         << "que vous souhaitez afficher." << endl
         << "Largeur : ";
    cin >> window.width_w;
    cout << "Hauteur : ";
    cin >> window.width_h;
    strcpy(window.name, "Système proie prédateur");
    return window;
}

/**
 * Function for config an ecosystem
 * @return
 */
void config_ecosystem(Ecosystem &ecosystem) {
    int tmp__size_x, tmp__size_y;
    bool stop = false;

    cout << "Configuration de l’écosystème." << endl
         << "==============================" << endl;
    cout << "Veuillez choisir la taille de la grille a utilisé." << endl;
    while (!stop) {
        cout << "Taille en x : ";
        cin >> tmp__size_x;
        cout << "Taille en y : ";
        cin >> tmp__size_y;
        if (tmp__size_x > GRID_WIDTH || tmp__size_y > GRID_WIDTH || tmp__size_y < 0 || tmp__size_x < 0) {
            throw invalid_argument("Les tailles rentrées ne sont pas valides, merci de réessayer.");
        } else {
            ecosystem.dx = tmp__size_x;
            ecosystem.dy = tmp__size_y;
            stop = true;
        }
    }

    // We can procedure for config the individual types.
    init_individual_types(ecosystem);
    config_individual_types(ecosystem);
    init_ecosystem_grid(ecosystem);
}


// DRAW FUNCTIONS

/**
 * Function for draw an individual.
 * @param individual
 */
void draw_individual(Individual individual, Position position, float ratio) {
    cout << position.x << " " << position.y << endl;
    if (is_herb(individual)) {
        if (is_old(individual)) {
            image_draw(individual.individual_type.images.male, position.x, position.y, ratio, ratio);
        } else {
            image_draw(individual.individual_type.images.child_male, position.x, position.y, ratio, ratio);
        }
    } else { // We must look the genre of the individual.
        if (is_male(individual)) {
            if (is_old(individual)) {
                image_draw(individual.individual_type.images.male, position.x, position.y, ratio, ratio);
            } else {
                image_draw(individual.individual_type.images.child_male, position.x, position.y, ratio, ratio);
            }
        } else {
            if (is_old(individual)) {
                image_draw(individual.individual_type.images.female, position.x, position.y, ratio, ratio);
            } else {
                image_draw(individual.individual_type.images.child_female, position.x, position.y, ratio, ratio);
            }
        }
    }
}

void draw_grid(Window &window) {
    float ratio = min(window.width_h / window.ecosystem.dy, window.width_w / window.ecosystem.dx);
    cout << ratio << endl;
    for (int i = 0; i < window.ecosystem.dx; ++i) {
        for (int j = 0; j < window.ecosystem.dy; ++j) {
            draw_individual(window.ecosystem.grid[i][j], make_position(ratio * i, ratio * j), ratio);
        }
    }
}

int main(int, char **) {
    Window w = config_window();

    winInit(w.name, w.width_w, w.width_h);
    config_ecosystem(w.ecosystem);
    winClear();

    draw_grid(w);
    pressSpace();
    winQuit();
    return 0;
}
