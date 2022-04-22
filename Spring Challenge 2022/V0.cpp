#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace std;

#define WIDTH 17630
#define HEIGHT 9000

#define WIND_RANGE 1280


class Entity {
    public:
        Entity(): handled(0), camping_pos("") {}
        ~Entity() {};

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

        int handled;
        string camping_pos;
};

typedef struct s_pos {
    int x;
    int y;
}   pos;

pos *pos_from_id(int id, vector<Entity> scene, const pos base, const Entity & hero)
{
    pos *res = new pos();

    if (base.x == 0)
    {
        res->x = base.x + (1130 / 3) * (1 + hero.id);
        res->y = base.y + 1130 - (1130 / 3) * (hero.id);
    }
    else
    {
        res->x = base.x - (1130 / 3) * ( 1 + hero.id);
        res->y = base.y - 1130 + (1130 / 3) * (hero.id);
    }

    for (vector<Entity>::iterator it = std::begin(scene); it != std::end(scene); ++it) {
        if ((*it).id == id)
        {
            res->x = (*it).x;
            res->y = (*it).y;
        }
    }
    return res;
}

int dist_to_base(Entity entity, const pos base)
{
    return (sqrt(pow(entity.x - base.x, 2) + pow(entity.y - base.y, 2)));
}

int dist_to_base_ennemy(Entity entity, const pos base)
{
    if (base.x == 0)
        return (sqrt(pow(entity.x - WIDTH, 2) + pow(entity.y - HEIGHT, 2)));
    else
        return (sqrt(pow(entity.x, 2) + pow(entity.y, 2)));
}

int monster_to_aim(vector<Entity> & scene, Entity & hero, const pos base)
{
    for (vector<Entity>::iterator it = std::begin(scene); it != std::end(scene); ++it) {
        if ((*it).type == 0 &&
            ((*it).handled == 0 || (*it).handled == hero.id) &&
            (*it).near_base == 1 &&
            (*it).threat_for == 1 &&
            (*it).health > 0)
            {
                (*it).handled = hero.id;
                return ((*it).id);
            }
        // else if ((*it).type == 0 &&
        //     ((*it).handled == 0 || (*it).handled == hero.id) &&
        //     dist_to_base(*it, base) < 3000 &&
        //     (*it).health > 0)
        //     {
        //         (*it).handled = hero.id;
        //         return ((*it).id);
        //     }
    }
    return (-1);
}

void    wind(const pos base, string type)
{
    if (!type.compare("def"))
        cout << "SPELL WIND " << WIDTH / 2 << " " << HEIGHT / 2 << endl;
    else {
        if (base.x == 0)
            cout << "SPELL WIND " << WIDTH << " " << HEIGHT << endl;
        else
            cout << "SPELL WIND " << "0" << " " << "0" << endl;
    }
}

int danger_to_avoid(vector<Entity> & scene, Entity & hero, const pos base)
{
    pos *aim;

    for (vector<Entity>::iterator it = std::begin(scene); it != std::end(scene); ++it) {
        if ((*it).type == 0 &&
            dist_to_base(*it, base) < 2000 &&
            (*it).health > 0)
            {
                aim = pos_from_id((*it).id, scene, base, *it);
                cout << "MOVE " << aim->x << " " << aim->y << endl;
                return ((*it).id);
            }
    }
    cout << "MOVE " << hero.camping_pos << endl;
    return (-1);
}

int critical_danger_monster(vector<Entity> & scene, Entity & hero, const pos base)
{
    for (vector<Entity>::iterator it = std::begin(scene); it != std::end(scene); ++it) {
        if ((*it).type == 0 &&
            dist_to_base(*it, base) < 1000 &&
            (*it).health > 0)
            {
                if (dist_to_base(hero, base) < WIND_RANGE) {
                    wind(base, "def");
                    return ((*it).id);
                }
                else {
                    cout << "MOVE " << hero.camping_pos << endl;
                    return ((*it).id);
                }
            }
    }
    return (-1);
}

int opportunity_to_attack(vector<Entity> & scene, Entity & hero, const pos base)
{
    for (vector<Entity>::iterator it = std::begin(scene); it != std::end(scene); ++it) {
        if ((*it).type == 0 &&
            (*it).health > 0 &&
            dist_to_base_ennemy(hero, base) < WIND_RANGE)
        {
            wind(base, "attack");
            return ((*it).id);
        }
        else {
            cout << "MOVE " << hero.camping_pos << endl;
            return ((*it).id);
        }
    }
    return (-1);
}

int main()
{
    int base_x; // The corner of the map representing your base
    int base_y;
    cin >> base_x >> base_y; cin.ignore();

    const pos base = {base_x, base_y};

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

        vector<Entity> scene;
        int hero_index = 1;

        for (int i = 0; i < entity_count; i++) {
            Entity  entity;

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
            int handled = 0;
            cin >> id >> type >> x >> y >> shield_life >> is_controlled >> health >> vx >> vy >> near_base >> threat_for; cin.ignore();

            entity.id = id;
            entity.type = type;
            entity.x = x;
            entity.y = y;
            entity.shield_life = shield_life;
            entity.health = health;
            entity.vx = vx;
            entity.vy = vy;
            entity.near_base = near_base;
            entity.near_base = threat_for;
            entity.near_base = handled;


            cerr << "id: " << id << ", type: " << type << endl;
            if (entity.type == 1)
            {
                if ((base.x == 0 && entity.id != 0) || (base.x != 0 && entity.id == 0))
                {
                    entity.camping_pos += to_string((1130 / 3) * ( 1 + entity.id));
                    entity.camping_pos += " ";
                    entity.camping_pos += to_string(1130 - (1130 / 3) * (entity.id));
                }
                else
                {
                    entity.camping_pos += to_string(WIDTH - (1130 / 3) * (1 + entity.id));
                    entity.camping_pos += " ";
                    entity.camping_pos += to_string(HEIGHT - 1130 + (1130 / 3) * (entity.id));
                }
                hero_index++;
            }
            scene.push_back(entity);
        }

        for (vector<Entity>::iterator it = std::begin(scene); it != std::end(scene); ++it) {
            if ((*it).type == 1)
            {
                pos *aim;
                if ((*it).id != 0) {
                    if (critical_danger_monster(scene, *it, base) != -1) {}
                    else if (danger_to_avoid(scene, *it, base) != -1) {}
                    else {
                        aim = pos_from_id(monster_to_aim(scene, *it, base), scene, base, *it);
                        cout << "MOVE " << aim->x << " " << aim->y << endl;
                    }
                }
                else {
                    if (opportunity_to_attack(scene, *it, base) != -1) {}
                    else
                        cout << "WAIT" << endl;
                }
            }
        }      
    }
}