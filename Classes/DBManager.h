#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "Tools.h"
#include "cocos2d.h"
USING_NS_CC;

typedef std::map <std::string, std::string> Row_t;
typedef std::vector <Row_t> Data_Rows_t;

class sqlite3;

struct WeaponsDBParams
{
    unsigned int id;
    unsigned int map_number;
    std::string code;
    int x;
    int y;
};

struct BulletsDBParams
{
    unsigned int id;
    unsigned int map_number;
    std::string code;
    int x;
    int y;
    unsigned int count;
};

enum class SimpleObjectType {HEALTH_POTION, NONE};

struct SimpleObjectDBParams
{
    unsigned int id;
    unsigned int map_number;
    SimpleObjectType type;
    std::string code;
    int x;
    int y;
};

struct EnemyDBParams
{
    unsigned int id;
    unsigned int map_number;
    std::string type_code;
    int base_x;
    int base_y;
    int x;
    int y;
    std::string weapon1;  // Может быть огнестрельным, может быть ближнего боя. 
    std::string weapon2;  // Может быть ближнего боя. Если weapon1 ближнего боя, то weapon2 не будет использовано.
    unsigned int bullets; // Если weapon1 огнестрельное, то здесь - количество патронов к нему.
    unsigned int strength;
    unsigned int accuracy;
    float angle;
    unsigned int damage;
};

class DBManager
{
public:
    static DBManager & I ();
    ~DBManager () = default;
    
private:
    DBManager ();
    DBManager (const DBManager & other) = delete;
    DBManager (const DBManager && other) = delete;
    DBManager & operator= (const DBManager & other) = delete;
    DBManager & operator= (const DBManager && other) = delete;
    
    std::string save_file_name = ""; // Полный путь с файлу сохранения.
    sqlite3 * pdb = nullptr;         // Указатель на соединение с БД в памяти.
    bool is_game_save_exist = false; // true, если существует файл сохранения.
    bool is_db_init = false;         // true после того, как БД будет заполнена исходными данными, либо загружена с сохранения.
    
    std::string simple_object_type_to_code (SimpleObjectType type);
    SimpleObjectType simple_object_code_to_type (const std::string & code);
    
    // Функция обратного вызова для SQLite.
    static int callback (void * rows, int argc, char ** argv, char ** colName);
    // Выполняется запрос. Во второй параметр записываются полученные строки.
    // Если второй параметр nullptr, то запрос без получения данных.
    void sql_query_exec (const char *, Data_Rows_t * const = nullptr);
    // Если save == true, то сохраняем данные с памяти на диск. Если false, то загружаем с диска в память.
    // Возвращаемое значение - успешно выполнена операция, или нет.
    bool load_or_save (bool save);
    // Запрос к БД, результат сохраняется в result.
    void select_values (const std::string & table, const std::string & where, Data_Rows_t * result);
    // Установка новых значений. В new_values пары - название поля и его новое значение.
    void set_values (const std::string & table, const Row_t & new_values, const std::string & where);
    
public:
    void init_default_values ();
    bool is_save_exist ();
    bool save ();
    bool load ();
    long int get_last_insert_id ();
    
    // Предназначено для получения числовых значений, а так же строки (специализация шаблона).
    template <typename T>
    T get_character_value (const std::string & value_name)
    {
        Data_Rows_t result;
        select_values("character_values", "code = \"" + value_name + "\"", & result);
        return (result.size() > 0) ? StringToNumber <T> (result[0]["value"]) : 0;
    }
    
    // Предназначено для установки числовых значений, а так же строк (специализация шаблона).
    // Кавычки в значениях недопустимы.
    template <typename T>
    void set_character_value (const std::string & value_name, const T & value)
    {
        Row_t row;
        row["value"] = NumberToString(value);
        set_values("character_values", row, "code = \"" + value_name + "\"");
    }
    
    std::vector <WeaponsDBParams> get_weapons_on_map (unsigned int map_number);
    std::vector <BulletsDBParams> get_bullets_on_map (unsigned int map_number);
    std::vector <SimpleObjectDBParams> get_simple_objects_on_map (unsigned int map_number);
    std::vector <EnemyDBParams> get_enemies_on_map (unsigned int map_number);
    
    // id в переданных параметрах не учитывается.
    void add_weapon_on_map (const WeaponsDBParams & param);
    void add_bullet_on_map (const BulletsDBParams & param);
    void add_simple_object_on_map (const SimpleObjectDBParams & param);
    void add_enemy_on_map (const EnemyDBParams & param);
    
    void delete_weapon_from_map (unsigned int id);
    void delete_bullet_from_map (unsigned int id);
    void delete_simple_object_from_map (unsigned int id);
    void delete_enemy_from_map (unsigned int id);
    
    EnemyDBParams get_enemy (unsigned int id);
    void update_enemy (const EnemyDBParams & param);
    void update_enemy_field (unsigned int id, const std::string & field_name, const std::string & value, bool need_quotes = true);
    
    template <typename T>
    T get_enemy_field (unsigned int id, const std::string & field_name)
    {
        Data_Rows_t result;
        std::string sql = "SELECT " + field_name + " FROM enemies WHERE id = " + NumberToString(id);
        sql_query_exec(sql.c_str(), & result);
        return StringToNumber <T> (result.empty() ? "0" : result[0].at(field_name));
    }
};

template <>
std::string DBManager::get_character_value <std::string> (const std::string & value_name);

template <>
void DBManager::set_character_value <std::string> (const std::string & value_name, const std::string & value);

#endif // DBMANAGER_H
