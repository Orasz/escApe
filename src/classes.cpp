#include "classes.h"
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// ROOM CLASS
room::room() {
  texture = "pics/foglia.jpg";
  empty = false;
  if (rand() % 100 < EMPTY_PROB)
    empty = true;
  ob = Objectt::Null;
}

room::room(int empty_prob) {
  texture = "pics/foglia.jpg"; // sfondo di ogni room
  empty = false;
  if (rand() % 100 < empty_prob) // setta probabilità che stanza sia vuota
    empty = true;
  ob = Objectt::Null; // oggetto di default = Null
}

bool room::isEmpty() { return empty; }

string room::getTexture() { return texture; }

void room::setObject(Objectt obj) { ob = obj; }

Objectt room::getObject() { return ob; }
// MAP CLASS

mappa::mappa() {
  raws = 7;
  cols = 11;
  outlinewidth = 10;
  room_dim.resize(2);
}

mappa::mappa(int rawss, int colss, int empty_proba) {
  raws = rawss; // setta numero iniziale righe e colonne della mappa
  cols = colss;
  empty_prob = empty_proba; // setta probabilità che le stanze siano vuote
  room_dim.resize(2);
  // scala dimensione mappa a seconda dello schermo
  map_dim[0] =
      (sf::VideoMode::getDesktopMode().width * (float)MAP_WIDTH_RATIO) / 100;
  map_dim[1] = sf::VideoMode::getDesktopMode().height;
  outlinewidth = 10;               // spazio fra le Rooms
  for (int i = 0; i < raws; i++) { // genera le rooms della mappa con
                                   // probabilità di default che siano vuote
    for (int k = 0; k < cols; k++) {
      if (i != 0 and i != raws - 1 and k != 0 and k != cols - 1)
        grid[i][k] = room(empty_prob);
      else // prima e ultima riga così come prima e ultima colonna la
           // probabilità che siano vuote è 0
        grid[i][k] = room(0);
    }
  }
}

vector<vector<reference_wrapper<room>>>
mappa::getGrid() { // metodo getter per il vettore bidimensionale grid
  vector<vector<reference_wrapper<room>>> rgrid(raws);
  for (int i = 0; i < raws; i++) {
    for (int k = 0; k < cols; k++) {
      rgrid[i].push_back(grid[i][k]);
    }
  }
  return rgrid;
}

int mappa::getRaws() { return raws; }

int mappa::getCols() { return cols; }

int mappa::getEmptyProb() { return empty_prob; }

int mappa::getOutLineWidth() { return outlinewidth; }

void mappa::setOutLineWidth(int val) { outlinewidth = val; }

vector<int> mappa::getRoomDim() { return room_dim; }

vector<vector<sf::RectangleShape>> mappa::generateMapShapes() {
  vector<vector<sf::RectangleShape>> shapes(
      raws); // genera matrice di oggetti shapes
  sf::Texture tmp;
  // calcola dimensioni quadrati/stanze prendendo quelle della mappa e
  // dividendole per il numero,dinamico, di righe e colonne
  room_dim[0] = (map_dim[0] - (cols * outlinewidth)) / cols;
  room_dim[1] = (map_dim[1] - (raws * outlinewidth)) / raws;
  // genera i quadrati e regola posizione, dimensione,  bordo e colore
  for (int i = 0; i < raws; i++) {
    for (int k = 0; k < cols; k++) {
      sf::RectangleShape tmp_shape(sf::Vector2f(room_dim[0], room_dim[1]));
      tmp_shape.setPosition(k * (room_dim[0] + outlinewidth) + outlinewidth,
                            i * (room_dim[1] + outlinewidth) + outlinewidth);
      tmp_shape.setOutlineThickness(outlinewidth);
      tmp_shape.setOutlineColor(sf::Color::Transparent);
      if (grid[i][k].isEmpty()) { // se la stanza è vuota essa non viene
                                  // riempita dalla texture e presenta
                                  // l'immagine di background
        tmp_shape.setFillColor(sf::Color::Transparent);
      }
      shapes[i].push_back(tmp_shape);
    }
  }
  return shapes;
}

// GAME CLASS
game::game() {
  srand(time(0));
  state = GameState::start;              // inizializza GameState
  map = mappa(NRAWS, NCOLS, EMPTY_PROB); // inizializza mappa
  level = 1; // livello 1, 1 vita, 1 oggetto all' interno della mappa
  lifes = 1;
  objs = 1;
  apesMoves = 0;
  bag = {};
  rooms = counterRooms(); // funzione che conta le stanze
  pg_cord.resize(2);      // funzione resize della libreria standard
  apesBadTurns.resize(1);
  bag.resize(0);
  apes = START_APES; // numero iniziale di scimmie
  apesBadTurns = {
      MAX_TURNS}; // vector utilizzato per inibire il movimento alle scimmie
  pg_cord = {0, 0};
  setApesCord();
  setStairsCord();
  setObjCord();
  window_dim[0] = sf::VideoMode::getDesktopMode().width;
  window_dim[1] = sf::VideoMode::getDesktopMode().height;
  modular_textures = false;
}
// funzione che prende come parametri una stringa e le sue coordinate e
// restituisce l'oggetto sf::Text
sf::Text game::getFormattedText(string s, int x, int y) {
  sf::Font font;
  sf::Text text;
  font.loadFromFile("pics/font.otf");
  text.setFont(font);
  text.setString(s);
  text.setCharacterSize(35);
  text.setColor(sf::Color::White);
  text.setStyle(sf::Text::Bold);
  text.setPosition(x, y);
  return text;
}
// Genera oggetto grafico sf::Sprite che descrive il personaggio
sf::Sprite game::getPgShape() {
  vector<int> room_dim = map.getRoomDim();
  int pg_pos[2];
  sf::Sprite pg;
  pg.setPosition(((room_dim[0] + map.getOutLineWidth()) * pg_cord[1]) +
                     ((room_dim[0] + map.getOutLineWidth()) * 0.25),
                 ((room_dim[1] + map.getOutLineWidth()) * pg_cord[0]) +
                     ((room_dim[1] + map.getOutLineWidth()) * 0.25));
  return pg;
}
// Restituisce il l'oggetto grafico sf::Sprite che rappresenta le scale
sf::Sprite game::getStairsSprite() {
  vector<int> room_dim = map.getRoomDim();
  sf::Sprite stairs;
  stairs.setPosition(((room_dim[0] + map.getOutLineWidth()) * stairs_cord[1]) +
                         ((room_dim[0] + map.getOutLineWidth()) * 0.15),
                     ((room_dim[1] + map.getOutLineWidth()) * stairs_cord[0]) +
                         ((room_dim[1] + map.getOutLineWidth()) * 0.15));
  return stairs;
}
// Restituisce un vettore di oggetti grafici che rappresentano le scimmie
vector<sf::Sprite> game::getApesSprite() {
  vector<sf::Sprite> sprites;
  vector<int> room_dim = map.getRoomDim();
  for (int i = 0; i < apes; i++) {
    sf::Sprite tmp;
    tmp.setPosition(((room_dim[0] + map.getOutLineWidth()) * apes_cord[i][1]) +
                        ((room_dim[0] + map.getOutLineWidth()) * 0.25),
                    ((room_dim[1] + map.getOutLineWidth()) * apes_cord[i][0]) +
                        ((room_dim[1] + map.getOutLineWidth()) * 0.25));
    sprites.push_back(tmp);
  }
  return sprites;
}
// Restituisce un vettore contenente gli oggetti grafici che rappresentano gli
// oggetti 'bonus'
vector<sf::Sprite> game::getObjectsSprites() {
  vector<sf::Sprite> sprites;
  vector<int> room_dim = map.getRoomDim();
  for (int i = 0; i < obj_cord.size(); i++) {
    sf::Sprite tmp;
    tmp.setPosition(((room_dim[0] + map.getOutLineWidth()) * obj_cord[i][1]) +
                        ((room_dim[0] + map.getOutLineWidth()) * 0.25),
                    ((room_dim[1] + map.getOutLineWidth()) * obj_cord[i][0]) +
                        ((room_dim[1] + map.getOutLineWidth()) * 0.25));
    sprites.push_back(tmp);
  }
  return sprites;
}
// assegna coordinate alle scimmie
void game::setApesCord() {
  apes_cord.resize(apes);
  for (int i = 0; i < apes; i++) {
    vector<int> tmp(2);
    do {
      tmp[0] = (rand() % (map.getRaws() - 4)) + 3;
      tmp[1] = (rand() % (map.getCols() - 4)) + 3;
      // coordinate non possono essere assegnate se la stanza è vuota o se vi è
      // già un'altra scimmia
    } while (map.getGrid()[tmp[0]][tmp[1]].get().isEmpty() or
             find(apes_cord.begin(), apes_cord.end(), tmp) != apes_cord.end());
    apes_cord[i] = tmp;
  }
}
// assegna coordinate delle scale, sempre nell'ultima colonna
void game::setStairsCord() {
  int stairs_raw = ((int)rand() % (map.getRaws() - 1)) + 1;
  stairs_cord = {stairs_raw, map.getCols() - 1};
}
// Funzione che si occupa del movimento delle scimmie (richiamata ad ogni turno)
void game::moveApes() {
  apesMoves++;
  vector<int> tmp;
  for (int i = 0; i < apes; i++) {
    Direction
        dir; // direzione viene stabilità calcolando distanza fra scimmie e pg
    tmp = apes_cord[i];
    if (abs(tmp[0] - pg_cord[0]) > abs(tmp[1] - pg_cord[1])) {
      if (tmp[0] - pg_cord[0] > 0)
        dir = Direction::up;
      else
        dir = Direction::down;
    } else {
      if (tmp[1] - pg_cord[1] > 0)
        dir = Direction::left;
      else
        dir = Direction::right;
    }
    apes_cord[i] = move(
        apes_cord[i],
        dir); // muove ogni scimmia nella direzione precedentemente stabilita
    if (operator==(apes_cord[i], stairs_cord)) // se le coordinate della scimmia
                                               // sono divenute le stesse delle
                                               // scale la scimmia rimane ferma
                                               // alla posizione precedente
      apes_cord[i] = tmp;
    if (operator==(apes_cord[i], tmp) and
        abs(tmp[0] - pg_cord[0]) <= abs(tmp[1] - pg_cord[1])) {
      if (tmp[0] - pg_cord[0] > 0)
        dir = Direction::up;
      else
        dir = Direction::down;
      apes_cord[i] = move(apes_cord[i], dir);
    }
    if (operator==(apes_cord[i], stairs_cord))
      apes_cord[i] = tmp;
  }
}
// funzione che prende input di movimento da tastiera e gestisce i possibili
// casi
void game::getAction(sf::Event event) {
  if (event.key.code == sf::Keyboard::Up)
    pg_cord = move(pg_cord, Direction::up);
  if (event.key.code == sf::Keyboard::Down)
    pg_cord = move(pg_cord, Direction::down);
  if (event.key.code == sf::Keyboard::Right)
    pg_cord = move(pg_cord, Direction::right);
  if (event.key.code == sf::Keyboard::Left)
    pg_cord = move(pg_cord, Direction::left);
  // se pg e oggetto hanno le stesse coordinate quest'ultimo è raccolto
  if (find(obj_cord.begin(), obj_cord.end(), pg_cord) != obj_cord.end() &&
      bag.size() < MAX_BAG) {
    pickUp(map.getGrid()[pg_cord[0]][pg_cord[1]]
               .get()
               .getObject()); // inserimento oggetto in borsa
    obj_cord.erase(
        find(obj_cord.begin(), obj_cord.end(), pg_cord)); // rimozione oggetto
    map.getGrid()[pg_cord[0]][pg_cord[1]].get().setObject(Objectt::Null);
  }
  // se tasto premuto è 1...5 usa relativo oggetto, che viene poi rimosso dalla
  // borsa
  if (event.text.unicode > 26 and event.text.unicode <= 26 + bag.size()) {
    useObjectt(bag[event.text.unicode - 27]);
    bag.erase(bag.begin() + event.text.unicode - 27);
  }
  // se è un "bad turn" le scimmie non muovono
  else if (find(apesBadTurns.begin(), apesBadTurns.end(), apesMoves) ==
           apesBadTurns.end())
    moveApes();
  else
    apesMoves++;
  // se pg e scale hanno stesse coordinate chiama funzione nextLevel
  if (operator==(pg_cord, stairs_cord))
    nextLevel();
  // se scimmie e pg hanno stesse coordinate diminuisci di 1 la vita, se scende
  // a 0 gameover.
  if (find(apes_cord.begin(), apes_cord.end(), pg_cord) != apes_cord.end()) {
    lifes = lifes - 1;
    // apes_cord.erase(find(apes_cord.begin(), apes_cord.end(), pg_cord));
    // apes --;
    useObjectt(Objectt::iceAge);
    if (lifes <= 0)
      state = GameState::gameover;
  }
}
// reinizializza tutto da capo
void game::restart() {
  pg_cord = {0, 0};
  level = 1;
  lifes = 1;
  objs = 1;
  bag = {};
  map = mappa(NRAWS, NCOLS, EMPTY_PROB);
  apes = START_APES;
  setApesCord();
  setStairsCord();
  setObjCord();
  rooms = counterRooms();
  state = GameState::guide;
}
// funzione chiamata se pg e scale hanno stesse coordinate, rigenera la mappa
// aumentando di 1 righe,colonne, oggetti e scimmie
void game::nextLevel() {
  pg_cord = {0, 0};
  int map_dim[2];
  int empty_prob;
  map_dim[0] = map.getRaws();
  map_dim[1] = map.getCols();
  empty_prob = map.getEmptyProb();
  map = mappa(map_dim[0] + 1, map_dim[1] + 1, empty_prob);
  apes++;
  level++;
  objs++;
  rooms = counterRooms();
  apesMoves = 0;
  apesBadTurns.resize(1);
  setApesCord();
  setStairsCord();
  setObjCord();
  state = GameState::active;
}
// assegna coordinate agli oggetti in modo casuale, in stanze vuote e ove non vi
// siano scimmmie
void game::setObjCord() {
  obj_cord.resize(objs);
  Objectt tmp;
  for (int i = 0; i < objs; i++) {
    vector<int> tmp(2);
    do {
      tmp[0] = (rand() % (map.getRaws() - 2)) + 1;
      tmp[1] = (rand() % (map.getCols() - 2)) + 1;
    } while (map.getGrid()[tmp[0]][tmp[1]].get().isEmpty() or
             find(apes_cord.begin(), apes_cord.end(), tmp) != apes_cord.end());
    std::vector<Objectt>
        toPick; // vettore da cui , in modo random, verrà preso l' oggetto
    toPick.push_back(Objectt::graveYard);
    toPick.push_back(Objectt::iceAge);
    toPick.push_back(Objectt::KingsOrder);
    toPick.push_back(Objectt::leftArrow);
    toPick.push_back(Objectt::rightArrow);
    toPick.push_back(Objectt::shuffleIt);
    toPick.push_back(Objectt::soHeavy);
    toPick.push_back(Objectt::straightToTheMoon);
    toPick.push_back(Objectt::topArrow);
    toPick.push_back(Objectt::byTheSkin);
    toPick.push_back(Objectt::downArrow);

    map.getGrid()[tmp[0]][tmp[1]].get().setObject(
        toPick[rand() % toPick.size()]);

    obj_cord[i] = tmp;
  }
}
// inserisce oggetto nella borsa se essa non è piena
void game::pickUp(Objectt ogg) {
  if (bag.size() < MAX_BAG)
    bag.push_back(ogg);
}
// funzione che "attiva" l' oggetto che ha come parametro
void game::useObjectt(Objectt obj) {
  switch (obj) {
  case (Objectt::leftArrow): { // le "Arrow" muovono le scimmie in direzione
                               // della freccia
    for (int i = 0; i < apes; i++)
      apes_cord[i] = move(apes_cord[i], Direction::left);
    break;
  }
  case (Objectt::downArrow): {
    for (int i = 0; i < apes; i++)
      apes_cord[i] = move(apes_cord[i], Direction::down);
    break;
  }

  case (Objectt::rightArrow): {
    for (int i = 0; i < apes; i++)
      apes_cord[i] = move(apes_cord[i], Direction::right);
    break;
  }

  case (Objectt::topArrow): {
    for (int i = 0; i < apes; i++)
      apes_cord[i] = move(apes_cord[i], Direction::up);
    break;
  }

  case (Objectt::graveYard): {
    // uccide la scimmia più vicina
    vector<int> tmp;
    int min = map.getRaws() + map.getCols();
    int min_idx = 0;
    for (int i = 0; i < apes; i++) {
      tmp = apes_cord[i];
      if (min > abs(pg_cord[0] - tmp[0]) + abs(pg_cord[1] - tmp[1])) {
        min = abs(pg_cord[0] - tmp[0]) + abs(pg_cord[1] - tmp[1]);
        min_idx = i;
      }
    }
    apes_cord.erase(apes_cord.begin() + min_idx);
    apes--;
    break;
  }

  case (Objectt::iceAge): { // "congela" tutte le scimmie per un turno
    apesBadTurns.push_back(apesMoves);
    break;
  }

  case (Objectt::KingsOrder): { // le scimmie muovono,se possibile, verso il
                                // centro
    vector<int> tmp;
    for (int i = 0; i < apes; i++) {
      Direction dir;
      tmp = apes_cord[i];
      if (abs(tmp[0] - (map.getRaws() / 2)) >
          abs(tmp[1] - (map.getCols() / 2))) {
        if (tmp[0] - (map.getRaws() / 2) > 0)
          dir = Direction::up;
        else
          dir = Direction::down;
      } else {
        if (tmp[1] - (map.getCols() / 2) > 0)
          dir = Direction::left;
        else
          dir = Direction::right;
      }
      apes_cord[i] = move(apes_cord[i], dir);
    }
    break;
  }

  case (Objectt::shuffleIt): { // riassegna le coordinate di tutte le scimmie
    apes_cord.clear();
    setApesCord();
    break;
  }

  case (Objectt::soHeavy): { // le scimmie muovono ogni 2 turni,ovvero quando
                             // apesMoves != apesBadTurns
    for (int i = apesMoves; i < MAX_TURNS; i = i + 2)
      apesBadTurns.push_back(i);
    break;
  }

  case (Objectt::straightToTheMoon): { // fai 3 mosse
    for (int i = apesMoves; i < apesMoves + 3; i++)
      apesBadTurns.push_back(i);
    break;
  }

  case (Objectt::byTheSkin): { // aumenta di 1 la vita
    lifes++;
    break;
  }
  }
}
// muove pg o scimmie(1o parametro) nella direzione indicata dal 2o parametro
vector<int> game::move(vector<int> cord, Direction dir) {
  vector<int> tmp_cord = cord;
  if (dir == Direction::up)
    tmp_cord[0] = cord[0] - 1;
  else if (dir == Direction::down)
    tmp_cord[0] = cord[0] + 1;
  else if (dir == Direction::right)
    tmp_cord[1] = cord[1] + 1;
  else if (dir == Direction::left)
    tmp_cord[1] = cord[1] - 1;
  // se coordinate andrebbero "fuori" dalla mappa o verso una stanza vuota o
  // verso una scimmia non muove
  if ((tmp_cord[0] >= 0 and tmp_cord[0] < map.getRaws()) and
      (tmp_cord[1] >= 0 and tmp_cord[1] < map.getCols()) and
      !map.getGrid()[tmp_cord[0]][tmp_cord[1]].get().isEmpty() and
      find(apes_cord.begin(), apes_cord.end(), tmp_cord) == apes_cord.end())
    return tmp_cord;
  return cord;
}

// conta numero di stanze non vuote
int game::counterRooms() {
  int counter = 0;
  for (int i = 0; i < map.getRaws(); i++)
    for (int k = 0; k < map.getCols(); k++)
      if (!map.getGrid()[i][k].get().isEmpty())
        counter++;
  return counter;
}

// Funzione grafica che ha il compito di gestire e mostrare a video tutto cio
// che la classe game organizza
void game::toPrint() {
  sf::Texture start_texture;
  sf::Texture guide_texture;
  sf::Texture background;
  sf::Texture pg_texture;
  sf::Texture ape_texture;
  sf::Texture stairs_texture;
  sf::Texture gameover_texture;
  sf::Texture sidemenu_texture;
  sf::Sprite background_sprite;
  sf::Sprite pg_shape;
  sf::Sprite stairs_sprite;
  sf::Sprite gameover_sprite;
  sf::Sprite start_sprite;
  sf::Sprite guide_sprite;
  sf::Sprite sidemenu_sprite;
  sf::Text lifes_text;
  sf::Text rooms_text;
  sf::Text apes_text;
  sf::Text level_text;
  sf::Font font;
  bool game_start = true;
  vector<sf::Sprite> ape_sprites;
  vector<sf::Sprite> obj_sprites;
  vector<sf::Sprite> bag_sprites;
  vector<sf::Texture> obj_textures;
  vector<vector<sf::Texture>> textures(int(NRAWS * 4));
  vector<vector<sf::RectangleShape>> shapes = map.generateMapShapes();
  vector<vector<reference_wrapper<room>>> grid = map.getGrid();
  vector<int> room_dim = map.getRoomDim();
  vector<string> obj_textures_names = {
      "graveYard.png",  "iceAge.png",
      "kingsOrder.png", "leftArrow.png",
      "rightArrow.png", "shuffle-it.png",
      "soHeavy.png",    "straightToTheMoon.png",
      "topArrow.png",   "byTheSkin.png",
      "downArrow.png"};
  // Texture loading
  // Rooms texture
  sf::Texture tmp_room;
  tmp_room.loadFromFile(grid[0][0].get().getTexture(),
                        sf::IntRect(0, 0, room_dim[0], room_dim[1]));
  for (int i = 0; i < int(NRAWS * 4); i++) {
    for (int k = 0; k < int(NCOLS * 3); k++) {
      if (modular_textures) {
        sf::Texture tmp;
        if (!tmp.loadFromFile(grid[i][k].get().getTexture(),
                              sf::IntRect(0, 0, room_dim[0], room_dim[1])))
          cout << "Texture opening errror";
      } else
        textures[i].push_back(tmp_room);
    }
  }

  for (int i = 0; i < int(Objectt::Null); i++) {
    sf::Texture tmp;
    tmp.loadFromFile("pics/" + obj_textures_names[i]);
    obj_textures.push_back(tmp);
  }
  pg_texture.loadFromFile("pics/pg.png");
  font.loadFromFile("pics/font.otf");
  start_texture.loadFromFile("pics/startScreen.png");
  guide_texture.loadFromFile("pics/objectGuide.png");
  background.loadFromFile("pics/background.jpg");
  stairs_texture.loadFromFile("pics/stairs.png");
  ape_texture.loadFromFile("pics/ape.png");
  sidemenu_texture.loadFromFile("pics/side_menu.png");
  gameover_texture.loadFromFile("pics/gameover.png");
  background_sprite.setTexture(background);
  sidemenu_sprite.setTexture(sidemenu_texture);
  auto size = background_sprite.getTexture()->getSize();
  background_sprite.setScale(float(window_dim[0]) / size.x,
                             float(window_dim[1]) / size.y);
  size = sidemenu_sprite.getTexture()->getSize();
  sidemenu_sprite.setScale(
      float(window_dim[0] * 0.01 * (100 - MAP_WIDTH_RATIO)) / size.x,
      float(window_dim[1]) / size.y);
  sidemenu_sprite.setPosition(float(window_dim[0] * MAP_WIDTH_RATIO * 0.01), 0);
  gameover_sprite.setTexture(gameover_texture);
  size = gameover_sprite.getTexture()->getSize();
  gameover_sprite.setScale(float(window_dim[0]) / size.x,
                           float(window_dim[1]) / size.y);
  start_sprite.setTexture(start_texture);
  size = start_sprite.getTexture()->getSize();
  start_sprite.setScale(float(window_dim[0]) / size.x,
                        float(window_dim[1]) / size.y);
  guide_sprite.setTexture(guide_texture);
  guide_sprite.setScale(float(window_dim[0]) / size.x,
                        float(window_dim[1]) / size.y);

  // creazione finestra
  sf::RenderWindow window(sf::VideoMode(window_dim[0], window_dim[1]),
                          "escAPE!", sf::Style::Fullscreen);
  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
      if (event.type == sf::Event::KeyPressed) {
        getAction(event);
        if (state == GameState::start)
          state = GameState::guide;

        else if (state == GameState::guide)
          state = GameState::active;
      }
      if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == 27)
          window.close();

        if (event.text.unicode == 114)
          restart();
        shapes = map.generateMapShapes();
      }
    }
    window.clear();
    window.draw(background_sprite);
    switch (state) {
    case (GameState::start): {
      window.draw(start_sprite);
      break;
    }
    case (GameState::guide): {
      window.draw(guide_sprite);
      break;
    }
    case (GameState::active): {
      auto menu_scale = sidemenu_sprite.getScale();
      auto menu_origin = sidemenu_sprite.getPosition();
      level_text =
          getFormattedText(to_string(level), menu_origin.x + menu_scale.x * 330,
                           0 + menu_scale.y * 370);
      apes_text =
          getFormattedText(to_string(apes), menu_origin.x + menu_scale.x * 300,
                           0 + menu_scale.y * 668);
      lifes_text =
          getFormattedText(to_string(lifes), menu_origin.x + menu_scale.x * 330,
                           0 + menu_scale.y * 468);
      rooms_text =
          getFormattedText(to_string(rooms), menu_origin.x + menu_scale.x * 330,
                           0 + menu_scale.y * 568);
      level_text.setFont(font);
      apes_text.setFont(font);
      lifes_text.setFont(font);
      rooms_text.setFont(font);
      grid = map.getGrid();
      shapes = map.generateMapShapes();
      for (int i = 0; i < map.getRaws(); i++) {
        for (int k = 0; k < map.getCols(); k++) {
          if (!grid[i][k].get().isEmpty())
            shapes[i][k].setTexture(&textures[i][k]);
          window.draw(shapes[i][k]);
        }
      }
      room_dim = map.getRoomDim();
      pg_shape = getPgShape();
      pg_shape.setTexture(pg_texture);
      size = pg_shape.getTexture()->getSize();
      pg_shape.setScale((float(room_dim[0]) / size.x) * 0.7,
                        (float(room_dim[1]) / size.y) * 0.7);
      stairs_sprite = getStairsSprite();
      stairs_sprite.setTexture(stairs_texture);
      size = stairs_sprite.getTexture()->getSize();
      stairs_sprite.setScale((float)room_dim[0] / (float)size.x,
                             (float)room_dim[1] / (float)size.y);
      ape_sprites = getApesSprite();
      obj_sprites = getObjectsSprites();
      for (int i = 0; i < apes; i++) {
        ape_sprites[i].setTexture(ape_texture);
        size = ape_sprites[i].getTexture()->getSize();
        ape_sprites[i].setScale((float(room_dim[0]) / size.x) * 0.7,
                                (float(room_dim[1]) / size.y) * 0.7);
        window.draw(ape_sprites[i]);
      }
      for (int i = 0; i < obj_cord.size(); i++) {
        obj_sprites[i].setTexture(
            obj_textures
                [(int)grid[obj_cord[i][0]][obj_cord[i][1]].get().getObject()]);
        size = obj_sprites[i].getTexture()->getSize();
        obj_sprites[i].setScale((float(room_dim[0]) / size.x) * 0.7,
                                (float(room_dim[1]) / size.y) * 0.7);
        window.draw(obj_sprites[i]);
      }
      for (int i = 0; i < bag.size(); i++) {
        sf::Sprite tmp;
        tmp.setPosition(menu_origin.x + menu_scale.x * (16 + (i * 120)),
                        0 + menu_scale.y * 902);
        tmp.setTexture(obj_textures[(int)bag[i]]);
        size = tmp.getTexture()->getSize();
        tmp.setScale((float(80) / size.x) * menu_scale.x,
                     (float(80) / size.y) * menu_scale.y);
        window.draw(tmp);
      }
      window.draw(pg_shape);
      window.draw(stairs_sprite);
      window.draw(sidemenu_sprite);
      window.draw(level_text);
      window.draw(lifes_text);
      window.draw(rooms_text);
      window.draw(apes_text);
      break;
    }
    case (GameState::gameover): {
      window.draw(gameover_sprite);
      break;
    }
    }
    window.display();
  }
}
