#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace std;

#define WIDTH 17630
#define HEIGHT 9000

#define WIND_RANGE 1280

typedef struct s_pos {
    int x;
    int y;
}   pos;

pos base;


string pos_to_str(pos *pos)
{
    string str;

    str += to_string(pos->x);
    str += " ";
    str += to_string(pos->y);

    return str;
}

class Entity {
    public:
        Entity(int id, int type, int x, int y, int shield_life, int is_controlled, int health, int vx, int vy, int near_base, int threat_for):
            id(id), type(type), x(x), y(y),
            shield_life(shield_life), is_controlled(is_controlled),
            health(health), vx(vx), vy(vy), near_base(near_base),
            threat_for(threat_for), handled(0), turn_played(0) {}
        ~Entity() {};

        int getType() const {return type;}
        int getId() const {return id;}
        string getCampingPos() const {return camping_pos;}
        void setCampingPos(string str) {camping_pos = str;}
        void setCampingPos(int x, int y) {
            pos *pos;
            pos->x = x; pos->y = y;
            camping_pos = pos_to_str(pos);}
        void setCampingPos(pos *pos) {camping_pos = pos_to_str(pos);}
        void playTurn() {turn_played = 1;}

        void goCamping() {
            string currPos;

            currPos = to_string(x) + " " + to_string(y);
            cerr << "curr pos: " << currPos << " and target: " << camping_pos << endl;
            if (currPos != camping_pos && !turn_played) {
                cout << "MOVE " << camping_pos << endl;
                this->playTurn();
            }
        }
        void cast_wind(const string type) {
            if (type == "defend" && !turn_played) {
                cout << "SPELL WIND " << WIDTH / 2 << " " << HEIGHT / 2 << endl;
                this->playTurn();
            }
            else if (type == "attack" && !turn_played) {
                cout << "SPELL WIND " << (base.x == 0 ? WIDTH : 0) << " " << (base.x == 0 ? HEIGHT : 0) << endl;
                this->playTurn();
            }
        }

        void wait() {
            if (!turn_played)
                cout << "WAIT ZzzzzZZz..." << endl;
        }

        int dist_base_ally() {return (sqrt(pow(x - base.x, 2) + pow(y - base.y, 2)));}
        int dist_base_ennemy() {return (sqrt(pow(x - (base.x ? 0 : WIDTH), 2) + pow(y - (base.x ? 0 : HEIGHT), 2)));}

        void playRole(vector<Entity *> scene) {
            if (!turn_played)
                return ;
            if (id == 0)
            {
                for (vector<Entity *>::iterator it = begin(scene); it != end(scene); ++it) {
                    if ((*it)->getType() == 0 && (*it)->dist_base_ennemy() < 1000) {
                        (*it)->cast_wind("attack");
                        (*it)->playTurn();
                    }
                }
            }
            else {
                for (vector<Entity *>::iterator it = begin(scene); it != end(scene); ++it) {
                    if ((*it)->getType() == 0 && (*it)->dist_base_ally() < 1000) {
                        (*it)->cast_wind("defend");
                        (*it)->playTurn();
                    }
                }
            }
        }


    private:
        int id; // Unique identifier
        const int type; // 0=monster, 1=your hero, 2=opponent hero
        int x; // Position of this entity
        int y;
        int shield_life; // Ignore for this league; Count down until shield spell fades
        int is_controlled; // Ignore for this league; Equals 1 when this entity is under a control spell
        int health; // Remaining health of this monster
        int vx; // Trajectory of this monster
        int vy;
        int near_base; // 0=monster with no target yet, 1=monster targeting a base
        int threat_for; // Given this monster's trajectory, is it a threat to 1=your base, 2=your opponent's base, 0=neither

        int handled; // only for monsters
        string camping_pos; // defenders and attackers near their respective bases
        int turn_played; // only for heroes
};



int main()
{
    int base_x; // The corner of the map representing your base
    int base_y;
    cin >> base_x >> base_y; cin.ignore();

    base.x = base_x; base.y = base_y;

    int heroes_per_player; // Always 3
    cin >> heroes_per_player; cin.ignore();

    // game loop
    while (1) {
        for (int i = 0; i < 2; i++) {
            int health; // Your base health
            int mana; // Ignore in the first league; Spend ten mana to cast a spell
            cin >> health >> mana; cin.ignore();
        }
        int entity_count; // Amount of heros and monsters you can see
        cin >> entity_count; cin.ignore();

        vector<Entity *> scene;
        for (int i = 0; i < entity_count; i++) {
            

            int id; // Unique identifier
            int type; // 0=monster, 1=your hero, 2=opponent hero
            int x; // Position of this entity
            int y;
            int shield_life; // Ignore for this league; Count down until shield spell fades
            int is_controlled; // Ignore for this league; Equals 1 when this entity is under a control spell
            int health; // Remaining health of this monster
            int vx; // Trajectory of this monster
            int vy;
            int near_base; // 0=monster with no target yet, 1=monster targeting a base
            int threat_for; // Given this monster's trajectory, is it a threat to 1=your base, 2=your opponent's base, 0=neither
            cin >> id >> type >> x >> y >> shield_life >> is_controlled >> health >> vx >> vy >> near_base >> threat_for; cin.ignore();

            Entity *entity = new Entity(id, type, x, y, shield_life, is_controlled, health, vx, vy, near_base, threat_for);
            
            if (entity->getType() == 1)
            {
                if ((base.x == 0 && entity->getId() != 0) || (base.x != 0 && entity->getId() == 0))
                {
                    entity->setCampingPos((1130 / 3) * ( 1 + entity->getId()), 1000 - (1000 / 3) * (entity->getId()));
                }
                else
                {
                    entity->setCampingPos(WIDTH - (1000 / 3) * (1 + entity->getId()), HEIGHT - 1000 + (1000 / 3) * (entity->getId()));
                }
            }
            scene.push_back(entity);
        }

        cerr << "prout" << endl;

        for (vector<Entity *>::iterator it = begin(scene); it != end(scene); ++it) {
            cerr << (*it)->getType() << endl;
            if ((*it)->getType() == 1)
            {
                cerr << (*it)->getCampingPos() << endl;
                (*it)->goCamping();
                (*it)->playRole(scene);
                (*it)->wait();
            }
        }      
    }
}