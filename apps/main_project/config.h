/**
 * Fichier contenant les entêtes des fonctions de configurations du
 * fichier config.
 * Auteur : Maxime Ben Hassen
 * Date : 04 avril 2021.
 */
#ifndef _CONFIG_H
#define _CONFIG_H

const int MAX_CHAR = 100;

/**
 * Struct for contains the size 
 * and the name of the window.
 */
struct Window
{
  char name[MAX_CHAR];
  int width_w, width_h;
};

Window set_config();

#endif _CONFIG_H