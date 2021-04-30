// apps/main_project/main.cpp
// Il cherchera à faire :
// - évoluer des populations dans un environnement
// - permettre de visualiser les évolutions de ces populations
// - tout ça dans un environnement modifiable et fléxible
// J'ai trouvé de la documentation ici :
// - https://en.wikipedia.org/wiki/Computational_science#Methods_and_algorithms
// - https://fr.wikipedia.org/wiki/%C3%89quations_de_pr%C3%A9dation_de_Lotka-Volterra
// - https://fr.wikipedia.org/wiki/%C3%89quilibres_pr%C3%A9dateurs-proies
// - https://github.com/ojroques/predator-prey-automaton

#include <Grapic.h>
#include <iostream>
#include <string.h>

using namespace grapic;
using namespace std;

// ===================
// | CONST VARIABLES |
// ===================

// Global utils
// ============
const int MAX_CHAR = 500;

// Window
const char WINDOW_NAME[MAX_CHAR] = "Système proie prédateur";

// MENU
// ====
const int MAX_BUTTON = 10;
const int MAX_INPUT = 20;
const int BUTTON_SIZE = 30;

// Ecosystem
// =========
const char BASE_DIR_IMG[MAX_CHAR] = "data/img/main_project/";
const char EXTENSION_IMG[MAX_CHAR] = ".png";
const int GRID_WIDTH = 20;
const char MENU_LABEL[MAX_INPUT][MAX_CHAR] = {"Nombre proie", "Duree de jeune proie", "Nombre predateur", "Duree de jeune predateur"};

// Individual type
const char TYPES[3][MAX_CHAR] = {"herb", "prey", "predator"};
const int YOUTH_DURATION[3] = {20, 3, 3};
const int LIFE_DURATION[3] = {100, 6, 6};

// Minimal age for kill some prey.
const int MIN_LIFE_DURATION_EAT = 2;

const int PERCENT_REPRODUCTION[3] = {0, 50, 100};
const int PERCENT_DEAD[3] = {0, 25, 25};
const int PERCENT_EAT[3] = {0, 0, 100};

// ===========
// | STRUCTS |
// ===========

typedef struct Window Window;

struct Position
{
    float x, y;
};

struct EmptyCases
{
    Position positions[8];
    int nb = 0;
};

struct IndividualTypeImages
{
    Image male;
    Image female;
    Image child_male;
    Image child_female;
};

/**
 * IndividualType is a struct for
 * define some data for a type.
 * int type, unique int for the IndividualType.
 * char name[MAX_CHAR] name of the type.
 * int life_duration maximum life span.
 * int youth_duration maximum youth duration.
 * int nb_entity number of this type in the ecosystem.
 * int percent_reproduction percentage chance that this type will be reproduced
 * int percent_dead percentage chance that this type will be dead
 * (when the life_duration of the individual > life_duration of the type).
 * int life_duration_eat min life_duration for eat an other type.
 */
struct IndividualType
{
    int type = 0;
    char name[MAX_CHAR] = "herb";

    int nb_entity = 0;

    // Min nb generation for can eat.
    int life_duration_eat = 0;

    // Max youth duration before start to did.
    int youth_duration = 100;

    // Max life_duration
    int life_duration = 100;

    // Percent of chance for is dead
    int percent_dead = 100;
    int percent_reproduction = 100;
    int percent_eat = 100;
};

/**
 * This struct represent an individual,
  * if genre is O, it is a man, else it is a female (1).
  * If individual_type is 0, it is a grass, if is 1, it is a prey, else it is a predator.
 */
struct Individual
{
    IndividualType individual_type;
    int genre = -1;
    int youth_duration = 0;
    int life_duration = 0;
    bool is_reproduced = false;
};

/**
 * This struct represent the ecosystem.
 */
struct Ecosystem
{
    Individual grid[GRID_WIDTH][GRID_WIDTH];
    IndividualType individual_types[3];
    IndividualTypeImages individual_type_images[3];
    int dx = 10, dy = 10;
};

struct Color
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};

struct Button
{
    Position min, max;
    Color color;
    char text[MAX_CHAR];

    void (*on_click)(Window &window);
};

struct NumberInput
{
    Button more_button;
    Button less_button;
    char content[MAX_CHAR];
    Color color;
};

struct PMenu
{
    NumberInput number_input[MAX_INPUT];
    int nb_input;
};

/**
 * This struct contains the size 
 * and the name of the window.
 */
struct Window
{
    char name[MAX_CHAR];
    int width_w, width_h;
    int ecosystem_w, ecosystem_h;
    int menu_w, menu_h;
    int graph_w, graph_h;
    Ecosystem ecosystem;
    Button start_button;
    PMenu menu;
};

// =====================
// | OPERATORS HEADERS |
// ====================

bool operator==(IndividualType individual_type1, IndividualType individual_type2);

bool operator==(Individual individual1, Individual individual2);

// ===========
// | GETTERS |
// ===========

// Position getters
// ================

/**
 * Function return true if the pos is empty (-1, -1).
 * @param position
 * @return
 */
bool is_empty_pos(Position position)
{
    return int(position.x) == -1 || int(position.y) == -1;
}

bool is_valid_pos(Position position, Ecosystem ecosystem)
{
    return !is_empty_pos(position) && int(position.x) < ecosystem.dx && int(position.y) < ecosystem.dy;
}

// Individual getters
// ==================

/**
 * Function for return if the individual1 can reproduced with the individual2.
 * @param individual1
 * @param individual2
 * @return
 */
bool can_reproduce_btw(Individual individual1, Individual individual2)
{
    return individual1.individual_type == individual2.individual_type && individual1.genre != individual2.genre &&
           !individual1.is_reproduced && !individual2.is_reproduced;
}

/**
 * Function for get an Individual by position.
 * @param ecosystem
 * @param position
 * @return
 */
Individual get_individual_by_position(Ecosystem ecosystem, Position position)
{

    if (int(position.x) < 0)
    {
        position.x = 0;
    }
    else if (int(position.x) >= ecosystem.dx)
    {
        position.x = ecosystem.dx - 1;
    }

    if (int(position.y) < 0)
    {
        position.y = 0;
    }
    else if (int(position.y) >= ecosystem.dy)
    {
        position.y = ecosystem.dy - 1;
    }

    return ecosystem.grid[int(position.x)][int(position.y)];
}

/**
 * function to verify that the individual is not too old to live or has been fasting too long.
 * @param individual
 * @return
 */
bool is_dead(Individual individual)
{
    return individual.youth_duration > individual.individual_type.youth_duration ||
           individual.life_duration > individual.individual_type.life_duration;
}

/**
 * Return true if the individual is female.
 * @param individual The individual analysed.
 * @return bool.
 */
bool is_female(Individual individual)
{
    if (individual.genre < 0)
    {
        throw invalid_argument("L’individu n’a pas de genre, ou celui-ci n’est pas defini.");
    }
    return individual.genre >= 1;
}

/**
 * This function return true
 * if the individual is a herb.
 * @param individual the individual analysed
 * @return bool
 */
bool is_herb(Individual individual)
{
    return individual.individual_type.type == 0;
}

/**
 * Return true if the individual is male.
 * @param individual The individual analysed.
 * @return bool.
 */
bool is_male(Individual individual)
{
    return !is_female(individual);
}

/**
 * Return if the individual is old (if his life_duration > LIFE_DURATION[type] / 2)
 * @param individual The individual analysed.
 * @return bool.
 */
bool is_old(Individual individual)
{
    return individual.life_duration > individual.individual_type.life_duration / 2;
}

/**
 * This function return true
 * if the individual is a predator.
 * @param individual the individual analysed
 * @return bool
 */
bool is_predator(Individual individual)
{
    return individual.individual_type.type == 2;
}

/**
 *
 * This function return true if
 * the Individual is an prey.
 * @param individual the individual analysed
 * @return bool
 */
bool is_prey(Individual individual)
{
    return individual.individual_type.type == 1;
}

/**
 * Function for return the type of an individual.
 * @param individual
 * @return
 */
int type(Individual individual)
{
    return individual.individual_type.type;
}

/**
 * Function return true if the individual can eat.
 * @param individual
 * @return
 */
bool can_eat(Individual individual)
{
    return is_predator(individual) && individual.life_duration > individual.individual_type.life_duration_eat;
}

// Button getters
// ==============

bool is_in(Position position, Button button)
{
    return position.x > button.min.x && position.x < button.max.x && position.y > button.min.y &&
           position.y < button.max.y;
}

// Ecosystem getters
// =================

bool is_full(Ecosystem ecosystem)
{
    return ecosystem.individual_types[1].nb_entity + ecosystem.individual_types[2].nb_entity >=
           ecosystem.dx * ecosystem.dy;
}

// Window getters
// ==============

Position get_mouse_pos()
{
    int x, y;
    mousePos(x, y);
    Position p;
    p.x = x;
    p.y = y;
    return p;
}

// ==================
// | MAKE FUNCTIONS |
// ==================

/**
 * Function for make a position (or Complex).
 * @param x
 * @param y
 * @return Position
 */
Position make_position(float x = -1, float y = -1)
{
    Position p;
    p.x = x;
    p.y = y;
    return p;
}

/**
 * Function for create a color.
 * @param r
 * @param g
 * @param b
 * @return
 */
Color make_color(unsigned char r = 255, unsigned char g = 0, unsigned char b = 0)
{
    Color color;
    color.red = r;
    color.green = g;
    color.blue = b;
    return color;
}

/**
 * Function for create button.
 * @param min_pos
 * @param max_pos
 * @param color
 * @return
 */
Button
make_button(Position min_pos, Position max_pos, void fn(Window &window), char text[MAX_CHAR],
            Color color = make_color())
{
    Button button;
    button.min = min_pos;
    button.max = max_pos;
    button.on_click = fn;
    button.color = color;
    strcpy(button.text, text);
    return button;
}

/**
 * Function for create an input.
 * @param min_pos
 * @param max_pos
 * @param color
 * @return
 */
NumberInput make_number_input(Position min_pos, Position max_pos, void fn1(Window &window), void fn2(Window &window),
                              char text1[MAX_CHAR], char text2[MAX_CHAR], char content[MAX_CHAR],
                              Color color = make_color())
{
    NumberInput number_input;
    number_input.more_button = make_button(min_pos, make_position(min_pos.x + ((max_pos.x - min_pos.x) / 2), max_pos.y),
                                           fn1,
                                           text1, color);
    number_input.less_button = make_button(make_position(min_pos.x + (max_pos.x - min_pos.x) / 2, min_pos.y),
                                           max_pos,
                                           fn2, text2, color);
    number_input.color = color;
    strcpy(number_input.content, content);
    return number_input;
}

/**
 * This function create an individual type images.
 * @param individual_type the type of individual for whom the images are to be created
 * @return IndividualTypeImages created
 */
IndividualTypeImages make_individual_type_images(int type)
{
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
    if (type > 0)
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
 * Function for create an individual type
 * @param type type
 * @return IndividualType
 */
IndividualType make_individual_type(int type)
{
    if (type < 0 || type > 3)
    {
        throw invalid_argument("The type of individu is not valide. It should be between 0 and 3.");
    }

    IndividualType individual_type;

    individual_type.type = type;
    strcpy(individual_type.name, TYPES[type]);
    individual_type.youth_duration = YOUTH_DURATION[type];
    individual_type.life_duration = LIFE_DURATION[type];
    individual_type.percent_reproduction = PERCENT_REPRODUCTION[type];
    individual_type.percent_dead = PERCENT_DEAD[type];
    individual_type.percent_eat = PERCENT_EAT[type];

    return individual_type;
}

/**
 * Function for make an individual.
 * @param individual_type IndividualType the type of individual created
 * @return Individual
 */
Individual make_individual(IndividualType individual_type, int genre = -1, int life_duration = 0)
{
    Individual individual;
    individual.individual_type = individual_type;
    if (is_prey(individual) || is_predator(individual))
    {

        if (genre == -1)
        {
            individual.genre = frand(0, 2);
        }
        else
        {
            individual.genre = genre;
        }

        if (life_duration > 0)
        {
            individual.life_duration = life_duration;
        }
    }
    return individual;
}

/**
 * Function for make an herb.
 * @param ecosystem
 * @return
 */
Individual make_herb(Ecosystem ecosystem)
{
    return make_individual(ecosystem.individual_types[0]);
}

/**
 * Function for make a predator.
 * @param ecosystem
 * @return
 */
Individual make_predator(Ecosystem ecosystem)
{
    return make_individual(ecosystem.individual_types[2]);
}

/**
 * Function for make a prey.
 * @param ecosystem
 * @return
 */
Individual make_prey(Ecosystem ecosystem)
{
    return make_individual(ecosystem.individual_types[1]);
}

// =============
// | OPERATORS |
// =============

// Individual operators
// ====================

/**
 * Return true if two individual are equal.
 * @param individual1
 * @param individual2
 * @return
 */
bool operator==(Individual individual1, Individual individual2)
{
    bool response = individual1.individual_type == individual2.individual_type;
    if (response && (is_prey(individual1) || is_predator(individual1)))
    {
        response = individual1.genre == individual2.genre;
    }
    return response;
}

// Individual type operators
// =========================

/**
 * Return true if two type are equal.
 * @param individual_type1
 * @param individual_type2
 * @return
 */
bool operator==(IndividualType individual_type1, IndividualType individual_type2)
{
    return individual_type1.type == individual_type2.type;
}

// Color operators
// ===============

Color operator+(Color color, float number)
{
    return make_color(color.red + number, color.green + number, color.blue + number);
}

// Position operators
// ==================

bool operator!=(Position p1, Position p2)
{
    return p1.x != p2.x && p1.y != p2.y;
}

// ===================
// | UTILS FUNCTIONS |
// ===================

// COLORS
// ======

/**
 * Utils function for set a color by a Struct Color.
 * @param c
 */
void color_by_struct(Color c)
{
    color(c.red, c.green, c.blue);
}

// ECOSYSTEM
// =========

/**
 * Function for compute ratio between the size of the window and the size of the ecosystem.
 * @param window
 * @return
 */
float compute_ratio_ecosystem(Window window)
{
    return min(window.ecosystem_h / (window.ecosystem.dy + 1), window.ecosystem_w / (window.ecosystem.dx + 1));
}

/**
 * Utils function for compute the sizes of different element of the window.
 * @param window
 */
void compute_window_sizes(Window &window)
{
    // Ecosystem size.
    window.ecosystem_w = (window.width_w * 2) / 3;
    window.ecosystem_h = (window.width_h * 2) / 3;

    // Menu size.
    window.menu_w = window.width_w / 3;
    window.menu_h = window.width_h;

    // Graph size.
    window.graph_w = (window.width_w * 2) / 3;
    window.graph_h = window.width_h / 3;
}

// CASES
// =====

/**
 * Function for add a position into an empty cases struct.
 * If the position < 0, we raise an error.
 * @param empty_cases
 * @param position
 */
void add_position_empty_case(EmptyCases &empty_cases, Position position)
{
    if (!is_empty_pos(position))
    {
        empty_cases.positions[empty_cases.nb] = position;
        empty_cases.nb++;
    }
}

/**
 * Function for find_empty_cases around a position
 * @param ecosystem
 * @param x
 * @param y
 * @return EmptyCases.
 */
EmptyCases find_cases(Ecosystem ecosystem, IndividualType individual_type, Position position = make_position())
{
    EmptyCases empty_cases;
    Position tmp_pos;

    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            tmp_pos = make_position(position.x + i, position.y + j);
            if (get_individual_by_position(ecosystem, tmp_pos).individual_type ==
                    individual_type &&
                tmp_pos != position)
            {
                add_position_empty_case(empty_cases, make_position(position.x + i, position.y + j));
            }
        }
    }
    return empty_cases;
}

/**
 * Function for find only one position.
 * @param ecosystem
 * @param x
 * @param y
 * @return Position.
 */
Position find_case(Ecosystem ecosystem, IndividualType individual_type, Position position = make_position())
{
    Position p = make_position();
    EmptyCases empty_cases = find_cases(ecosystem, individual_type, position);
    if (empty_cases.nb > 0)
    {
        p = empty_cases.positions[irand(0, empty_cases.nb - 1)];
    }

    return p;
}

/**
 * Function for find a random case in the ecosystem with an inidividual_type.
 * @param ecosystem
 * @param individual_type
 * @return
 */
Position find_random_case(Ecosystem ecosystem, IndividualType individual_type)
{
    Position p = make_position();
    if (is_predator(make_individual(individual_type)) || is_prey(make_individual(individual_type)))
    {
        while (is_empty_pos(p) && ecosystem.individual_types[individual_type.type].nb_entity > 0)
        {
            int y = irand(1, ecosystem.dy - 1);
            int x = irand(1, ecosystem.dx - 1);
            p = find_case(ecosystem, individual_type, make_position(x, y));
        }
    }
    else
    {
        while (is_empty_pos(p) && !is_full(ecosystem))
        {
            int y = irand(1, ecosystem.dy - 1);
            int x = irand(1, ecosystem.dx - 1);
            p = find_case(ecosystem, individual_type,
                          make_position(x, y));
        }
    }
    return p;
}

/**
 * Function for find a case with an individual_type in two ecosystem.
 * @param ecosystem1
 * @param ecosystem2
 * @param individual_type
 * @param position
 * @return
 */
EmptyCases find_double_ecosystem_cases(Ecosystem ecosystem1, Ecosystem ecosystem2, IndividualType individual_type,
                                       Position position = make_position())
{
    EmptyCases empty_cases;
    Position current_pos;
    Individual indivi_eco1;
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            current_pos = make_position(position.x + i, position.y + j);
            indivi_eco1 = get_individual_by_position(ecosystem1, current_pos);
            if (indivi_eco1.individual_type == individual_type &&
                get_individual_by_position(ecosystem2, current_pos) == indivi_eco1)
            {
                add_position_empty_case(empty_cases, make_position(position.x + i, position.y + j));
            }
        }
    }
    return empty_cases;
}

/**
 * Function for find a position in two ecosystem.
 * @param ecosystem1
 * @param ecosystem2
 * @param individual_type
 * @param position
 * @return
 */
Position find_double_ecosystem_case(Ecosystem ecosystem1, Ecosystem ecosystem2, IndividualType individual_type,
                                    Position position = make_position())
{
    EmptyCases empty_cases = find_double_ecosystem_cases(ecosystem1, ecosystem2, individual_type, position);
    Position p = make_position();
    if (empty_cases.nb > 0)
    {
        p = empty_cases.positions[int(frand(0, empty_cases.nb))];
    }

    return p;
}

/**
 * Function for get if an individual can be reproduce
 * @param individual
 * @param ecosystem
 * @param x
 * @param y
 * @return bool. (true, if the individual was reproduced).
 */
Position find_couple_position(Ecosystem ecosystem, Position position)
{
    Position repro_position = make_position();
    for (int i = -1; i < 2; ++i)
    {
        for (int j = -1; j < 2; ++j)
        {
            if (can_reproduce_btw(ecosystem.grid[int(position.x)][int(position.y)],
                                  ecosystem.grid[int(position.x + i)][int(position.y + j)]))
            {
                repro_position = make_position(position.x + i, position.y + j);

                i = 1;
                j = 1;
            }
        }
    }
    return repro_position;
}

/**
 * Function for search a button with the mouse position in the window.
 * @param mouse_position
 * @param window
 */
void search_button(Position mouse_position, Window &window)
{
    if (is_in(mouse_position, window.start_button))
        window.start_button.on_click(window);
    else
    {
        for (int i = 0; i < window.menu.nb_input; ++i)
        {
            if (is_in(mouse_position, window.menu.number_input[i].less_button))
                window.menu.number_input[i].less_button.on_click(window);
            else if (is_in(mouse_position, window.menu.number_input[i].more_button))
                window.menu.number_input[i].more_button.on_click(window);
        }
    }
}

// STRING
// ======

int length_str(char string[MAX_CHAR])
{
    int i = 0;
    while (string[i] != '\0')
    {
        i++;
    }
    return i;
}

// ====================
// | UPDATE FUNCTIONS |
// ====================

// ECOSYSTEM
// =========

/**
 * Function for add an individual into the ecosystem.
 * @param ecosystem
 * @param individual
 * @param position
 */
void add_individual_ecosystem(Ecosystem &ecosystem, Individual individual, Position position)
{
    try
    {
        if (is_valid_pos(position, ecosystem))
        {
            Individual current_indivi = get_individual_by_position(ecosystem, position);
            if (!is_herb(current_indivi))
            {
                ecosystem.individual_types[type(current_indivi)].nb_entity--;
            }
            ecosystem.grid[int(position.x)][int(position.y)] = individual;
            ecosystem.individual_types[type(individual)].nb_entity++;
        }
        else
        {
            throw invalid_argument("La position pour ajouter l’individu n’est pas valide.");
        }
    }
    catch (const exception &e)
    {
        cout << e.what() << endl;
    }
}

/**
 * Function for kill an individual (at a position) in the ecosystem
 * @param ecosystem
 * @param position
 */
void kill_individual_ecosystem(Ecosystem &ecosystem, Position position)
{
    try
    {
        if (is_valid_pos(position, ecosystem))
        {
            ecosystem.individual_types[type(get_individual_by_position(ecosystem, position))].nb_entity--;
            ecosystem.grid[int(position.x)][int(position.y)] = make_herb(ecosystem);
        }
        else
        {
            throw invalid_argument("La position pour tuer l‘invididu n’est pas valide.");
        }
    }
    catch (const exception &e)
    {
        cout << e.what() << endl;
    }
}

/**
 * Function for eat a prey with the position of the predator and the position of the prey.
 * @param ecosystem
 * @param predator_pos
 * @param prey_pos
 */
void eat_individual_ecosystem(Ecosystem &ecosystem, Position predator_pos, Position prey_pos)
{
    if (is_predator(get_individual_by_position(ecosystem, predator_pos)) &&
        is_prey(get_individual_by_position(ecosystem, prey_pos)))
    {
        kill_individual_ecosystem(ecosystem, prey_pos);
        ecosystem.grid[int(predator_pos.x)][int(predator_pos.y)].youth_duration = 0;
    }
}

/**
 * Function for move an individual
 * into the ecosystem.
 * @param ecosystem
 * @param old_position
 * @param new_position
 */
Position move_individual_ecosystem(Ecosystem &ecosystem, Position old_position, Position new_position)
{
    Position p = old_position;
    if (is_herb(get_individual_by_position(ecosystem, new_position)))
    {
        ecosystem.grid[int(new_position.x)][int(new_position.y)] = get_individual_by_position(ecosystem, old_position);
        ecosystem.grid[int(old_position.x)][int(old_position.y)] = make_herb(ecosystem);
        p = new_position;
    }
    return p;
}

/**
 * Function for reproduce two individual into an ecosystem.
 * @param ecosystem
 * @param indivi1_pos
 * @param indivi2_pos
 */
void reproduce_individual_ecosystem(Ecosystem &ecosystem, Position indivi1_pos, Position indivi2_pos, Position new_pos)
{
    add_individual_ecosystem(ecosystem,
                             make_individual(ecosystem.individual_types[type(
                                 get_individual_by_position(ecosystem, indivi1_pos))]),
                             new_pos);
    ecosystem.grid[int(indivi1_pos.x)][int(indivi1_pos.y)].is_reproduced = true;
    ecosystem.grid[int(indivi2_pos.x)][int(indivi2_pos.y)].is_reproduced = true;
}

/**
 * function for fill the ecosystem.
 * @param ecosystem Ecosystem to fill.
 */
void fill_ecosystem_grid(Ecosystem &ecosystem)
{
    int x, y;
    for (int i = 1; i < 3; ++i)
    {
        for (int j = 0; j < ecosystem.individual_types[i].nb_entity; ++j)
        {
            do
            {
                x = irand(1, (ecosystem.dx - 1));
                y = irand(1, (ecosystem.dy - 1));
            } while (!is_herb(ecosystem.grid[x][y]));
            ecosystem.grid[x][y] = make_individual(ecosystem.individual_types[i]);
        }
    }
}

/**
 * Function for update an individual.
 * @param ecosystem
 * @param indivi_pos
 */
void update_individual(Ecosystem ecosystem, Ecosystem &new_ecosystem, Position indivi_pos)
{
    Position p = find_case(new_ecosystem, ecosystem.individual_types[0], indivi_pos);

    Individual indivi_current = get_individual_by_position(ecosystem, indivi_pos);
    if (!is_empty_pos(p))
    {
        Position repro_pos = find_couple_position(new_ecosystem, indivi_pos);
        if (!is_empty_pos(repro_pos) &&
            ecosystem.individual_types[type(indivi_current)].percent_reproduction > frand(0, 100))
        {
            reproduce_individual_ecosystem(new_ecosystem, indivi_pos, repro_pos, p);
        }
        else
        {
            indivi_pos = move_individual_ecosystem(new_ecosystem, indivi_pos, p);

            // if a prey move, his jeune duration = 0
            if (is_prey(indivi_current))
            {
                new_ecosystem.grid[int(p.x)][int(p.y)].youth_duration = 0;
            }
        }
    }

    if (is_dead(indivi_current) && ecosystem.individual_types[type(indivi_current)].percent_dead > frand(0, 100))
    {
        kill_individual_ecosystem(new_ecosystem, indivi_pos);
    }
    else
    {
        new_ecosystem.grid[int(indivi_pos.x)][int(indivi_pos.y)].youth_duration++;
        new_ecosystem.grid[int(indivi_pos.x)][int(indivi_pos.y)].life_duration++;
    }
}

void update_individual_type_metadata(Ecosystem &ecosystem)
{
    for (int i = 0; i < ecosystem.dx; i++)
    {
        for (int j = 0; j < ecosystem.dy; j++)
        {
            if (is_prey(ecosystem.grid[i][j]) || is_predator(ecosystem.grid[i][j]))
            {
                ecosystem.grid[i][j].individual_type.youth_duration = ecosystem.individual_types[type(ecosystem.grid[i][j])].youth_duration;
                ecosystem.grid[i][j].individual_type.life_duration = ecosystem.individual_types[type(ecosystem.grid[i][j])].life_duration;
                ecosystem.grid[i][j].individual_type.life_duration_eat = ecosystem.individual_types[type(ecosystem.grid[i][j])].life_duration_eat;
            }
        }
    }
}

/**
 * Function for reset all individuals in the ecosystem.
 * @param ecosystem
 */
void pre_update(Ecosystem &ecosystem)
{
    for (int i = 0; i < ecosystem.dx; ++i)
    {
        for (int j = 0; j < ecosystem.dy; ++j)
        {
            ecosystem.grid[i][j].is_reproduced = false;
        }
    }
}

/**
 * Function for update prey.
 * @param ecosystem
 */
void update_prey(Ecosystem ecosystem, Ecosystem &new_ecosystem)
{
    for (int i = 0; i < ecosystem.dx; ++i)
    {
        for (int j = 0; j < ecosystem.dy; ++j)
        {
            Position indivi_pos = make_position(i, j);
            if (is_prey(get_individual_by_position(ecosystem, indivi_pos)))
            {
                update_individual(ecosystem, new_ecosystem, indivi_pos);
            }
        }
    }
}

/**
 * Function for update predator.
 * @param ecosystem
 */
void update_predator(Ecosystem ecosystem, Ecosystem &new_ecosystem)
{
    for (int i = 0; i < ecosystem.dx; ++i)
    {
        for (int j = 0; j < ecosystem.dy; ++j)
        {
            Position indivi_pos = make_position(i, j);
            Individual current_individu = get_individual_by_position(ecosystem, indivi_pos);
            if (is_predator(current_individu))
            {
                Position indivi_eat_pos = find_double_ecosystem_case(ecosystem, new_ecosystem,
                                                                     ecosystem.individual_types[1], indivi_pos);

                if (!is_empty_pos(indivi_eat_pos) &&
                    can_eat(current_individu) && ecosystem.individual_types[type(current_individu)].percent_eat > frand(0, 100))
                {
                    eat_individual_ecosystem(ecosystem, indivi_pos, indivi_eat_pos);
                }
                else
                {
                    update_individual(ecosystem, new_ecosystem, indivi_pos);
                }
            }
        }
    }
}

/**
 * Function for update all the ecosystem.
 * @param ecosystem
 */
void update_grid(Ecosystem &ecosystem)
{
    pre_update(ecosystem);
    Ecosystem new_ecosystem = ecosystem;
    update_prey(ecosystem, new_ecosystem);
    update_predator(ecosystem, new_ecosystem);
    ecosystem = new_ecosystem;
}

void update_ecosystem(Window &window)
{
    for (int i = 0; i < 1; ++i)
    {
        update_grid(window.ecosystem);
        delay(200);
    }
}

void mouse_press(Window &window)
{
    Position mouse_pos = get_mouse_pos();
    search_button(mouse_pos, window);
    delay(200);
}

void update_window(Window &window)
{
    if (isMousePressed(SDL_BUTTON_LEFT))
        mouse_press(window);
}

// MENU
// ====

void add_random_prey(Window &window)
{
    Position p = find_random_case(window.ecosystem, window.ecosystem.individual_types[0]);
    add_individual_ecosystem(window.ecosystem, make_prey(window.ecosystem), p);
}

void kill_random_prey(Window &window)
{
    kill_individual_ecosystem(window.ecosystem,
                              find_random_case(window.ecosystem, window.ecosystem.individual_types[1]));
}

void less_youth_duratuion_prey(Window &window)
{
    if (window.ecosystem.individual_types[1].youth_duration > 0)
    {
        window.ecosystem.individual_types[1].youth_duration--;
        update_individual_type_metadata(window.ecosystem);
    }
}

void more_youth_duration_prey(Window &window)
{
    window.ecosystem.individual_types[1].youth_duration++;
    update_individual_type_metadata(window.ecosystem);
}

void add_random_predator(Window &window)
{
    add_individual_ecosystem(window.ecosystem, make_predator(window.ecosystem), find_random_case(window.ecosystem, window.ecosystem.individual_types[0]));
}

void kill_random_predator(Window &window)
{
    kill_individual_ecosystem(window.ecosystem, find_random_case(window.ecosystem, window.ecosystem.individual_types[2]));
}

void less_youth_duration_predator(Window &window)
{
    if (window.ecosystem.individual_types[2].youth_duration > 0)
    {
        window.ecosystem.individual_types[2].youth_duration--;
        update_individual_type_metadata(window.ecosystem);
    }
}

void more_youth_duration_predator(Window &window)
{
    window.ecosystem.individual_types[2].youth_duration++;
    update_individual_type_metadata(window.ecosystem);
}

// ==================
// | INIT FUNCTIONS |
// ==================

/**
 * Function for init each individual types.
 * @param ecosystem Ecosystem to which the type of individual belongs
 */
void init_individual_types(Ecosystem &ecosystem)
{
    for (int i = 0; i < 3; i++)
    {
        ecosystem.individual_types[i] = make_individual_type(i);
    }
}

/**
 * Function for fill individual_type_image[3]
 * @param individual_type_images
 */
void init_individual_types_image(IndividualTypeImages individual_type_images[3])
{
    for (int i = 0; i < 3; i++)
    {
        individual_type_images[i] = make_individual_type_images(i);
    }
}

/**
 * Function for init the ecosystem grid with the first type.
 * @param ecosystem Ecosystem to init.
 */
void init_ecosystem_grid(Ecosystem &ecosystem)
{
    for (int i = 0; i < ecosystem.dx; ++i)
    {
        for (int j = 0; j < ecosystem.dy; ++j)
        {
            ecosystem.grid[i][j] = make_individual(ecosystem.individual_types[0]);
        }
    }
}

/**
 * Function for init the window and create the menu.
 * @param window
 */
void init_buttons(Window &window)
{
    void (*on_clicks[MAX_INPUT * 2])(Window & window) = {
        add_random_prey, kill_random_prey,
        more_youth_duration_prey, less_youth_duratuion_prey,
        add_random_predator, kill_random_predator,
        more_youth_duration_predator, less_youth_duration_predator};

    char text1[MAX_CHAR];
    char text2[MAX_CHAR];
    strcpy(text1, "Start simulation");
    window.start_button = make_button(make_position(window.width_w - window.menu_w, 0),
                                      make_position(window.width_w, BUTTON_SIZE),
                                      update_ecosystem, text1);

    window.menu.nb_input = 4;

    for (int i = 0; i < window.menu.nb_input; ++i)
    {
        strcpy(text1, "+");
        strcpy(text2, "-");
        char label[MAX_CHAR];
        strcpy(label, MENU_LABEL[i]);

        window.menu.number_input[i] = make_number_input(
            make_position(window.width_w - window.menu_w, (BUTTON_SIZE * (i + 1)) + BUTTON_SIZE / 10),
            make_position(window.width_w, BUTTON_SIZE * (i + 2)), on_clicks[i * 2],
            on_clicks[i * 2 + 1],
            text1, text2, label);
    }
}

// ====================
// | CONFIG FUNCTIONS |
// ===================

/**
 * Procedure to config an individual_type
 * @param ecosystem Ecosystem of the inidvidual_type
 */
void config_individual_types(Ecosystem &ecosystem)
{
    bool stop = false;
    int tmp__nb_type = -1, tmp__nb_type_2 = -1;
    cout << "Configuration des types d’individus." << endl
         << "-----------------------------------" << endl;
    cout << "Veuillez choisir le nombre de d’individu de chaque type que vous souhaitez." << endl;
    while (!stop)
    {
        try
        {
            cout << "Nombre de proie (maximum : " << (ecosystem.dx - 2) * (ecosystem.dy - 2)
                 << ") : ";
            cin >> tmp__nb_type;

            cout << "Nombre de predateur (maximum : " << (ecosystem.dx - 2) * (ecosystem.dy - 2) - tmp__nb_type
                 << ") : ";
            cin >> tmp__nb_type_2;

            if (tmp__nb_type <= 0 || tmp__nb_type_2 <= 0 ||
                tmp__nb_type + tmp__nb_type_2 >= (ecosystem.dx - 2) * (ecosystem.dy - 2))
            {
                throw invalid_argument("Les nombres rentres ne sont pas valides, merci de reessayer.");
            }
            else
            {
                stop = true;
                ecosystem.individual_types[0].nb_entity =
                    (ecosystem.dx * ecosystem.dy) - (tmp__nb_type + tmp__nb_type_2);
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
 * @return Window
 */
Window config_window()
{
    Window window;
    cout << "Configuration de la fenetre." << endl
         << "============================" << endl;
    cout << "Bienvenue sur mon systeme proie-predateur, merci de definir la taille de la fenetre "
         << "que vous souhaitez afficher." << endl
         << "Largeur : ";
    cin >> window.width_w;
    cout << "Hauteur : ";
    cin >> window.width_h;
    strcpy(window.name, WINDOW_NAME);
    compute_window_sizes(window);
    return window;
}

/**
 * Function for config an ecosystem
 * @return
 */
void config_ecosystem(Ecosystem &ecosystem)
{
    int tmp__size_x = 0, tmp__size_y = 0;
    bool stop = false;

    cout << "Configuration de l’ecosysteme." << endl
         << "==============================" << endl;
    cout << "Veuillez choisir la taille de la grille a utilise." << endl;
    while (!stop)
    {
        try
        {
            cout << "Taille en x (maximum : " << GRID_WIDTH << ") : ";
            cin >> tmp__size_x;
            cout << "Taille en y (maximum : " << GRID_WIDTH << ") : ";
            cin >> tmp__size_y;
            if (tmp__size_x > GRID_WIDTH || tmp__size_y > GRID_WIDTH || tmp__size_y < 0 || tmp__size_x < 0)
            {
                throw invalid_argument("Les tailles rentrees ne sont pas valides, merci de reessayer.");
            }
            else
            {
                ecosystem.dx = tmp__size_x;
                ecosystem.dy = tmp__size_y;
                stop = true;
            }
        }
        catch (const exception &e)
        {
            cout << e.what() << endl;
        }
    }

    init_individual_types_image(ecosystem.individual_type_images);

    // We can procedure for config the individual types.
    init_individual_types(ecosystem);
    config_individual_types(ecosystem);

    init_ecosystem_grid(ecosystem);
    fill_ecosystem_grid(ecosystem);
}

// ==================
// | DRAW FUNCTIONS |
// ==================

/**
 * Function for draw the data of the ecosystem.
 * @param ecosystem
 */
void draw_ecosystem_data(Window window)
{
    char menu_label[MAX_CHAR];
    int i;
    for (i = 1; i < 3; ++i)
    {
        strcpy(menu_label, MENU_LABEL[i - 1]);
        print(window.graph_w - length_str(menu_label) * 15, window.width_h - (i * i * 20), menu_label);
        print(window.graph_w, window.width_h - (i * i * 20), window.ecosystem.individual_types[i].nb_entity);
        strcpy(menu_label, MENU_LABEL[i - 1]);
        print(window.graph_w - length_str(menu_label) * 15, window.width_h - (i * 20 + 20 * i), menu_label);
        print(window.graph_w, window.width_h - (i * 20 + 20 * i), window.ecosystem.individual_types[i].youth_duration);
    }
}

/**
 * Function for draw an individual.
 * @param individual individual to draw
 * @param position position of the individual
 * @param ratio size of the image.
 */
void draw_individual(Individual individual, IndividualTypeImages images, Position position, float ratio)
{
    if (is_herb(individual))
    {
        color(0, 200, 100);
        rectangleFill(int(position.x), int(position.y), int(position.x + ratio), int(position.y + ratio));
    }
    else // We must look the genre of the individual.
    {

        if (is_male(individual))
        {
            if (is_old(individual))
            {
                image_draw(images.male, position.x, position.y, ratio, ratio);
            }
            else
            {
                image_draw(images.child_male, position.x, position.y, ratio, ratio);
            }
        }
        else
        {
            if (is_old(individual))
            {
                image_draw(images.female, position.x, position.y, ratio, ratio);
            }
            else
            {
                image_draw(images.child_female, position.x, position.y, ratio, ratio);
            }
        }
    }
    color(200, 0, 0);
    rectangle(int(position.x), int(position.y), int(position.x + ratio), int(position.y + ratio));
}

void draw_button(Button button)
{
    color_by_struct(button.color);
    rectangleFill(int(button.min.x), int(button.min.y), int(button.max.x), int(button.max.y));
    color_by_struct(button.color + 10);
    rectangle(int(button.min.x), int(button.min.y), int(button.max.x), int(button.max.y));
    print(int((button.min.x + button.max.x) / 2 - length_str(button.text) * 6), int(button.min.y), button.text);
}

void draw_number_input(NumberInput number_input)
{
    color_by_struct(number_input.color);
    rectangleFill(int(number_input.more_button.min.x), int(number_input.more_button.min.y),
                  int(number_input.less_button.max.x), int(number_input.less_button.max.y));
    draw_button(number_input.more_button);
    draw_button(number_input.less_button);
    print(int(
              (number_input.less_button.max.x + number_input.more_button.min.x) / 2 -
              length_str(number_input.content) * 6),
          int(number_input.less_button.min.y), number_input.content);
}

/**
 * Function for draw the ecosystem.
 * @param window
 */
void draw_ecosystem(Window window)
{
    backgroundColor(0, 200, 100);
    float ratio = compute_ratio_ecosystem(window);
    for (int i = 0; i < window.ecosystem.dx; ++i)
    {
        for (int j = 0; j < window.ecosystem.dy; ++j)
        {
            draw_individual(window.ecosystem.grid[i][j],
                            window.ecosystem.individual_type_images[type(window.ecosystem.grid[i][j])],
                            make_position(ratio * i, ratio * j), ratio);
        }
    }
    draw_ecosystem_data(window);
}

void draw_window(Window window)
{
    draw_ecosystem(window);

    draw_button(window.start_button);

    for (int i = 0; i < window.menu.nb_input; ++i)
    {
        draw_number_input(window.menu.number_input[i]);
    }
}

int main(int, char **)
{
    bool stop = false;
    srand(time(NULL));
    Window w = config_window();

    winInit(w.name, w.width_w, w.width_h);
    setKeyRepeatMode(false);

    config_ecosystem(w.ecosystem);
    init_buttons(w);

    while (!stop)
    {

        winClear();

        draw_window(w);

        update_window(w);

        stop = winDisplay();
    }

    return 0;
}
