#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "bdd.h"
#include "utils.h"

#define bool int
#define false ((bool)0)
#define true ((bool)1)

//Nom du fichier contenant les données
static const char *DATA = "data";
static const char *OLDDATA = "data.old";

//Déclaration d'un flux
FILE *fdata;
FILE *oldfdata;

//Retourne une string à partir d'un Day
char *day_to_string(enum Day d)
{
  switch (d)
  {
  case MON:
    return "Lundi";
  case TUE:
    return "Mardi";
  case WED:
    return "Mercredi";
  case THU:
    return "Jeudi";
  case FRI:
    return "Vendredi";
  case SAT:
    return "Samedi";
  case SUN:
    return "Dimanche";
  case NONE:
    return "Not a day";
  }
}

//Retourne un Day à partir d'un string
//dans le cas où la string ne correspond pas à un jour, on renvoie NONE
enum Day string_to_day(char *dd)
{
  char d[LINE_SIZE];
  strcpy(d, dd);
  //Conversion en minuscule
  for (int i = 0; i < strlen(d); i++)
    d[i] = tolower(d[i]);

  if (strcmp("lundi", d) == 0)
    return MON;
  else if (strcmp("mardi", d) == 0)
    return TUE;
  else if (strcmp("mercredi", d) == 0)
    return WED;
  else if (strcmp("jeudi", d) == 0)
    return THU;
  else if (strcmp("vendredi", d) == 0)
    return FRI;
  else if (strcmp("samedi", d) == 0)
    return SAT;
  else if (strcmp("dimanche", d) == 0)
    return SUN;
  else
    return NONE;
}

// Libère la mémoire d'un pointeur vers Data
void data_free(Data *d)
{
  free(d->name);
  free(d->activity);
  free(d);
}

//Modifie une chaîne de caratère correspondant à data
void data_format(char *l, Data *data)
{
  sprintf(l, "%s,%s,%s,%d\n",
          data->name, data->activity,
          day_to_string(data->day), data->hour);
}

//Retourne une structure Data à partir d'une ligne de donnée
// get_data("toto,arc,lundi,4") ->  Data { "toto", "arc", MON, 4 };
// Attention il faudra libérer la mémoire vous-même après avoir utilisé
// le pointeur généré par cette fonction
Data *get_data(char *line)
{
  char *parse;
  Data *data = malloc(sizeof(Data));
  char error_msg[LINE_SIZE];
  sprintf(error_msg, "Erreur de parsing pour: %s\n", line);

  //On s'assure que la ligne qu'on parse soit dans le mémoire autorisée en
  // écriture
  char *l = malloc(strlen(line) + 1);
  l = strncpy(l, line, strlen(line) + 1);

  parse = strtok(l, ",");
  if (parse == NULL)
    exit_msg(error_msg, 0);
  data->name = malloc(strlen(parse) + 1);
  strcpy(data->name, parse);

  parse = strtok(NULL, ",");
  if (parse == NULL)
    exit_msg(error_msg, 0);
  data->activity = malloc(strlen(parse) + 1);
  strcpy(data->activity, parse);

  parse = strtok(NULL, ",");
  if (parse == NULL)
    exit_msg(error_msg, 0);
  data->day = string_to_day(parse);

  parse = strtok(NULL, "\n");
  if (parse == NULL)
    exit_msg(error_msg, 0);
  data->hour = atoi(parse);
  free(l);

  return data;
}

//La fonction _add_data_  retourne 0 si l'opération s'est bien déroulé
//sinon -1
int add_data(Data *data)
{
  fdata = fopen(DATA, "a");
  if (fdata == NULL)
  {
    return -1;
  }
  char dataline[LINE_SIZE];
  data_format(dataline, data);
  fputs(dataline, fdata);
  if (fclose(fdata) == EOF)
  {
    return -1;
  }
  printf("Commande effectuée\n");
  return 0;
}

//Enlève la donnée _data_ de la base de donnée
// TODO:
// il y a une particularité à cette fonction, comme ça n'a pas été présenté par l'énoncé
// j'ai fait le choix de supprimer la première donnée qui corresponds à celle donnée
// Autrement, le code va supprimer tous les exemples de la database qui correspondent à la valeur donnée
// Après ça reste un behavior attendu comme généralement on donne un ID à la bdd (ce qui n'est pas le cas ici)
// et donc avec la data structure fournie, on ne peut assigner un chevalier à plus d'une activité sur une heure !
int delete_data(Data *data)
{
  remove("data.old");
  if (rename("data", "data.old") != 0)
    return -1;
  oldfdata = fopen(OLDDATA, "r");
  fdata = fopen(DATA, "a");
  if (fdata == NULL || oldfdata == NULL)
    return -1;
  char line[LINE_SIZE];
  char trashline[LINE_SIZE];
  data_format(trashline, data);
  bool deleted = 0;
  while (fgets(line, LINE_SIZE, oldfdata))
  {
    if (strcmp(line, trashline) == 0 && !deleted)
    {
      deleted = 1;
    }
    else
    {
      fputs(line, fdata);
    }
  }
  remove("data.old");
  if (fclose(fdata) == EOF)
  {
    return -1;
  }
  if (fclose(oldfdata) == EOF)
  {
    return -1;
  }
  printf("Commande effectuée\n");
  return 0;
}

void read_one_line(char *l, Data *data)
{
  sprintf(l, "%s à %dh: %s a %s\n", day_to_string(data->day), data->hour, data->name, data->activity);
}

//Affiche le planning
int see_all(char *answer)
{
  fdata = fopen(DATA, "r");
  if (fdata == NULL)
    return 1;
  char line[LINE_SIZE];
  while (fgets(line, LINE_SIZE, fdata))
  {
    Data *data = get_data(line);
    char one_line[LINE_SIZE];
    read_one_line(one_line, data);
    strcat(answer, one_line);
  }
  if (fclose(fdata) == EOF)
    return 1;
  return 0;
}

int main(int argc, char **argv)
{
  // delete_data(get_data("toto,arc,mardi,4"));
  if (strcmp(argv[1], "ADD") == 0)
  {
    char dataline[LINE_SIZE];
    sprintf(dataline, "%s,%s,%s,%s", argv[2], argv[3], argv[4], argv[5]);
    add_data(get_data(dataline));
    return 0;
  }
  else if (strcmp(argv[1], "DEL") == 0)
  {
    char dataline[LINE_SIZE];
    sprintf(dataline, "%s,%s,%s,%s", argv[2], argv[3], argv[4], argv[5]);
    delete_data(get_data(dataline));
    return 0;
  }
  else if (strcmp(argv[1], "SEE") == 0)
  {
    char answer[5 * LINE_SIZE];
    see_all(answer);
    printf("%s", answer);
    return 0;
  }
  else
  {
    printf("%s\n", argv[1]);
    printf("Command not found : %s\n", argv[1]);
    return 1;
  }
}
