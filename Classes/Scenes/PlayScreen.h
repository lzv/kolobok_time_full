#ifndef PLAYSCREEN_H
#define PLAYSCREEN_H

#include "GameScreen.h"
#include "../AlgorithmJPS_v2.h"
#include "../BoolBitMap.h"
#include "../Interface.h"

class Character;
class Enemy;
class MapObject;
class MapActor;

enum class PointType {NOT_ACCESSIBLE, ACCESSIBLE, OBJECT, CHARACTER, ENEMY};

class PlayScreen : public GameScreen 
{
public:
    CREATE_FUNC (PlayScreen);
    bool init ();
    
    PointType getPointType (const Point & click_point);
    MapObject * getLastFoundMapObject ();
    Enemy * getLastFoundEnemy ();
    Point getLayersPos ();
    void deleteMapObject (MapObject * obj);
    void addMapObject (MapObject * obj);
    void updateInterface ();
    bool isIntersectMapActorWithMoveObstructions (MapActor * actor);
    void on_attack (MapActor * actor); // Обработка попадания и урона.
    JPSFindResults find_patch_from_MapActor (MapActor * actor, const Point & end_map_point);
    void set_pause (bool is_pause);
    
protected:
    PlayScreen () = default;
    virtual ~PlayScreen ();
    
    unsigned int _current_map_number;
    Character * _character = nullptr;
    std::map <std::string, Layer *> _layers;
    BoolBitMap _move_bit_map;                  // Битовая карта с препятствиями для движения.
    BoolBitMap _move_bit_map_find_patch;       // Битовая карта для алгоритма поиска пути.
    BoolBitMap _attack_bit_map;                // Битовая карта с препятствиями для атаки.
    unsigned int _bit_map_width = 0;
    unsigned int _bit_map_height = 0;
    Size map_content_size, window_size;
    std::vector <MapObject *> _map_objects;
    std::vector <Enemy *> _enemies;
    Interface _interface;
    SpriteFrame * _black_pixel = nullptr;
    bool _on_pause = false;
    
    // Запоминаем объекты, найденные при последнем вызове getPointType.
    MapObject * _last_found_map_object = nullptr;
    Enemy * _last_found_enemy = nullptr;
    
    void onCharacterDead ();
    void onPauseGame ();
    
    virtual void update (float delta);
    virtual void onEnterTransitionDidFinish () override;
    
private:
    void create_layers (const Size & map_content_size, const Size & window_content_size);
    void set_layers_position ();
    void create_move_bit_map_find_patch (const std::string & file_name);
    void show_bullet_line (Point start, Point end);
    
    DELETE_COPY_AND_MOVE(PlayScreen);
    
    friend class EnemyWait;
    friend class EnemyReturnToBase;
    friend class EnemyNoAttack;
};

#endif // PLAYSCREEN_H
