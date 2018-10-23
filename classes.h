#include <string>
#include <iostream>
#include <vector>
#include <memory>
#define MAX_BAG 5
#define MAP_WIDTH_RATIO 80
#define EMPTY_PROB 32
#define START_APES 1
#define NRAWS 6
#define NCOLS 9
#define MAX_TURNS 100000
using namespace std;
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>


enum class Direction {up, down, left, right};

enum class GameState {active, gameover, start, guide};

enum class Objectt {graveYard = 0, iceAge = 1, KingsOrder = 2, leftArrow = 3, rightArrow = 4, shuffleIt = 5, soHeavy = 6, straightToTheMoon = 7, topArrow = 8, byTheSkin = 9, downArrow = 10, Null = 11};

class room{
	protected:
		int empty;
		string texture;
		Objectt ob;
	public:
		room();
		room(int empty_prob);
		void setObject(Objectt obj);
		Objectt getObject();
		string getTexture();
		bool isEmpty();
};

class mappa{
	protected:
		room grid[int(NRAWS * 4)][int(NCOLS * 3)];
		int cols;
		int raws;
		int outlinewidth;
		int empty_prob;
		vector<int> room_dim;
		int map_dim[2];

	public:
		mappa();
		mappa(int raws, int cols, int empty_proba);
		int getCols();
		int getRaws();
		int getEmptyProb();
		int getOutLineWidth();
		void setOutLineWidth(int val);
		vector<int> getRoomDim();
		vector<vector<reference_wrapper<room>> > getGrid();
		vector<vector<sf::RectangleShape> > generateMapShapes();


};

class game{
	protected:
		mappa map;
		int level;
		int rooms;
		int apes;
		int objs;
		vector<Objectt> bag;
		int window_dim[2];
		int apesMoves;
	  int lifes;
		bool modular_textures;
		vector<int> pg_cord;
	  vector<int> apesBadTurns;
		vector<vector<int> > apes_cord;
		vector<vector<int> > obj_cord;
		vector<int> stairs_cord;
		GameState state;
	public:
		game();
		void getAction(sf::Event);
		void toPrint();
		void setApesCord();
		void setStairsCord();
		void setObjCord();
	  int counterRooms();
		void moveApes();
		void restart();
		void nextLevel();
		void pickUp(Objectt obj);
		void useObjectt(Objectt obj);
		sf::Sprite getPgShape();
		sf::Sprite getStairsSprite();
		sf::Text getFormattedText(string s, int x, int y);
		vector<sf::Sprite> getApesSprite();
		vector<sf::Sprite> getObjectsSprites();
		vector<int> move(vector<int> cord, Direction dir);
};
