#include "DBManager.h"
#include "sqlite3.h"
#include <exception>

static const std::map <SimpleObjectType, std::string> simple_object_db_codes {
    {SimpleObjectType::HEALTH_POTION, "health_potion"}
};

DBManager & DBManager::I ()
{
    static DBManager obj;
    return obj;
}

DBManager::DBManager ()
{
    if (sqlite3_open(":memory:", & pdb) != SQLITE_OK) throw std::runtime_error("Can't create db in memory.");
    auto sears_patches = FileUtils::getInstance()->getSearchPaths();
    if (sears_patches.size() == 0) throw std::runtime_error("Not found the search pathes.");
    save_file_name = sears_patches[0] + "game.save";
    is_game_save_exist = FileUtils::getInstance()->isFileExist(save_file_name);
}

std::string DBManager::simple_object_type_to_code (SimpleObjectType type)
{
    if (simple_object_db_codes.find(type) != simple_object_db_codes.end())
        return simple_object_db_codes.at(type);
    return "";
}

SimpleObjectType DBManager::simple_object_code_to_type (const std::string & code)
{
    for (auto & val : simple_object_db_codes)
        if (val.second == code) return val.first;
    return SimpleObjectType::NONE;
}

int DBManager::callback (void * rows, int argc, char ** argv, char ** colName)
{
    Row_t row;
    for (int i = 0; i < argc; i++) row[std::string(colName[i])] = std::string(argv[i]);
    static_cast <Data_Rows_t *> (rows)->push_back(row);
    return 0;
};

void DBManager::sql_query_exec (const char * sql, Data_Rows_t * const rows)
{
    int exec_result = rows ? sqlite3_exec(pdb, sql, callback, rows, nullptr) : sqlite3_exec(pdb, sql, nullptr, nullptr, nullptr);
    if (exec_result != SQLITE_OK) throw std::runtime_error("DB query error: " + std::string(sql));
}

void DBManager::init_default_values ()
{
    // Если в БД уже есть данные, очищаем ее с помощью переподключения.
    if (is_db_init)
    {
        sqlite3_close(pdb);
        pdb = nullptr;
        if (sqlite3_open(":memory:", & pdb) != SQLITE_OK) throw std::runtime_error("Can't create db in memory.");
    }
    // Создаем таблицы, заполняем стартовыми значениями.
    // В БД хранятся только те данные, которые могут измениться, и которые включаются в сохранение игры.
    const char * init_queries [] {
        "CREATE TABLE character_values (code TEXT PRIMARY KEY, value TEXT)",
        "CREATE TABLE weapons_on_maps (id INTEGER PRIMARY KEY, map_number INTEGER, weapon_code TEXT, "
                    "pos_x INTEGER, pos_y INTEGER)",
        "CREATE TABLE bullets_on_maps (id INTEGER PRIMARY KEY, map_number INTEGER, bullet_code TEXT, "
                    "pos_x INTEGER, pos_y INTEGER, count INTEGER)",
        "CREATE TABLE simple_objects (id INTEGER PRIMARY KEY, map_number INTEGER, type TEXT, code TEXT, "
                    "pos_x INTEGER, pos_y INTEGER)",
        "CREATE TABLE enemies (id INTEGER PRIMARY KEY, map_number INTEGER, type_code TEXT, base_x INTEGER, base_y INTEGER, "
                    "pos_x INTEGER, pos_y INTEGER, weapon1 TEXT, weapon2 TEXT, bullets INTEGER, strength INTEGER, "
                    "accuracy INTEGER, angle REAL, damage INTEGER)"
    };
    for (auto q : init_queries) sql_query_exec(q);
    
    // character_values (code TEXT PRIMARY KEY, value TEXT)
    const std::map <std::string, std::string> character_values {
        {"strength",        "1"},           /* Сила */
        {"accuracy",        "1"},           /* Точность */
        {"free_CP",         "1"},           /* Свободные поинты характеристик. */
        {"experience",      "0"},           /* Опыт */
        {"hits_damage",     "0"},           /* Текущий урон у хитов персонажа */
        {"level",           "1"},           /* Текущий уровень */
        {"map_number",      "1"},           /* Текущая карта */
        {"pos_x",           "107.0"},       /* Текущая координата x */
        {"pos_y",           "739.0"},       /* Текущая координата y */
        {"pistol_bullets",  "0"},           /* Количество патронов для пистолета */
        {"machine_bullets", "0"},           /* Количество патронов для автомата */
        {"weapon",          "small_sword"}, /* Текущее оружие */
        {"rotate_angle",    "0.0"},         /* Угол поворота от вектора (1, 0) по часовой стрелке в градусах */
    };
    
    // weapons_on_maps (map_number INTEGER, weapon_code TEXT, pos_x INTEGER, pos_y INTEGER)
    const std::vector <std::vector <std::string>> weapon_db_params {
        {"1", "long_sword", "730", "500"},
        {"1", "axe", "633", "1600"},
        {"1", "postol", "1660", "1600"},
        {"1", "mashine", "1356", "1100"}
    };
    
    // bullets_on_maps (map_number INTEGER, bullet_code TEXT, pos_x INTEGER, pos_y INTEGER, count INTEGER)
    const std::vector <std::vector <std::string>> bullets_db_params {
        {"1", "pistol_bullet", "830", "800", "15"},
        {"1", "machine_bullet", "1563", "1813", "25"},
        {"1", "machine_bullet", "1345", "1442", "31"}
    };
    
    // simple_objects (map_number INTEGER, type TEXT, code TEXT, pos_x INTEGER, pos_y INTEGER)
    std::string hp_code = simple_object_type_to_code(SimpleObjectType::HEALTH_POTION);
    const std::vector <std::vector <std::string>> simple_objects_db_params {
        {"1", hp_code, "health_small", "350", "700"},
        {"1", hp_code, "health_small", "1800", "1450"},
        {"1", hp_code, "health_big", "1635", "544"},
        {"2", hp_code, "health_big", "50", "40"}
    };
    
    // enemies (map_number INTEGER, type_code TEXT, base_x INTEGER, base_y INTEGER, pos_x INTEGER, pos_y INTEGER, 
    // weapon1 TEXT, weapon2 TEXT, bullets INTEGER, strength INTEGER, accuracy INTEGER, angle REAL, damage INTEGER)
    const std::vector <std::vector <std::string>> enemies_db_params {
        {"1", "green", "812", "1266", "812", "1266", "small_sword", "", "0", "1", "2", "0", "0"},
        {"1", "blue", "1378", "1416", "1378", "1416", "axe", "", "0", "2", "2", "0", "0"},
        {"1", "braun", "1420", "596", "1420", "596", "postol", "small_sword", "5", "2", "2", "0", "0"},
        {"3", "pink", "300", "450", "300", "450", "long_sword", "", "0", "3", "2", "0", "0"},
        {"3", "red", "450", "300", "450", "300", "long_sword", "", "0", "3", "2", "0", "0"}
    };
    
    std::string insert_sql = "BEGIN; ";
    for (const std::pair <std::string, std::string> & val: character_values)
    {
        insert_sql += "INSERT INTO character_values (code, value) ";
        insert_sql += "VALUES (\"" + val.first + "\", \"" + val.second + "\"); ";
    }
    for (const auto & vals : weapon_db_params)
    {
        insert_sql += "INSERT INTO weapons_on_maps (map_number, weapon_code, pos_x, pos_y) ";
        insert_sql += "VALUES (" + vals[0] + ", \"" + vals[1] + "\", " + vals[2] + ", " + vals[3] + "); ";
    }
    for (const auto & vals : bullets_db_params)
    {
        insert_sql += "INSERT INTO bullets_on_maps (map_number, bullet_code, pos_x, pos_y, count) ";
        insert_sql += "VALUES (" + vals[0] + ", \"" + vals[1] + "\", " + vals[2] + ", " + vals[3] + ", " + vals[4] + "); ";
    }
    for (const auto & vals : simple_objects_db_params)
    {
        insert_sql += "INSERT INTO simple_objects (map_number, type, code, pos_x, pos_y) ";
        insert_sql += "VALUES (" + vals[0] + ", \"" + vals[1] + "\", \"" + vals[2] + "\", " + vals[3] + ", " + vals[4] + "); ";
    }
    for (const auto & vals : enemies_db_params)
    {
        insert_sql += "INSERT INTO enemies (map_number, type_code, base_x, base_y, pos_x, pos_y, "
                                            "weapon1, weapon2, bullets, strength, accuracy, angle, damage) ";
        insert_sql += "VALUES (" + vals[0] + ", \"" + vals[1] + "\", " + vals[2] + ", " + vals[3] + 
                      ", " + vals[4] + ", " + vals[5] + ", \"" + vals[6] + "\", \"" + vals[7] + "\", " + 
                      vals[8] + ", " + vals[9] + ", " + vals[10] + ", " + vals[11] + ", " + vals[12] + "); ";
    }
    insert_sql += "COMMIT;";
    sql_query_exec(insert_sql.c_str());
    
    is_db_init = true;
}

bool DBManager::is_save_exist ()
{
    return is_game_save_exist;
}

void DBManager::select_values (const std::string & table, const std::string & where, Data_Rows_t * result)
{
    std::string sql = "SELECT * FROM " + table;
    if (where.length() > 0) sql += " WHERE " + where;
    sql_query_exec(sql.c_str(), result);
}

void DBManager::set_values (const std::string & table, const Row_t & new_values, const std::string & where)
{
    std::string sql = "UPDATE " + table + " SET ";
    unsigned int ind = 0, new_values_count = new_values.size();
    for (auto it = new_values.begin(); it != new_values.end(); ++it)
    {
        sql += it->first + " = \"" + it->second + "\"";
        if (ind < new_values_count - 1) sql += ", ";
        ++ind;
    }
    if (where.length() > 0) sql += " WHERE " + where;
    sql_query_exec(sql.c_str());
}

bool DBManager::load_or_save (bool save)
{
    int result_code;          // Код результата. 
    sqlite3 * pFile;          // Соединение с БД на диске. 
    sqlite3_backup * pBackup; // Объект для копирования данных. 
    sqlite3 * pTo;            // Источник копирования (pdb или pFile). 
    sqlite3 * pFrom;          // Получатель данных (pFile или pdb). 
    
    // Сохранять можно только инициализированную БД. Загружать можно, только если сохранение существует.
    if ((save && is_db_init) || (!save && is_game_save_exist))
    {
        result_code = sqlite3_open(save_file_name.c_str(), &pFile);
        if (result_code == SQLITE_OK)
        {
            pFrom = (save ? pdb   : pFile);
            pTo   = (save ? pFile : pdb);
            pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");
            if (pBackup)
            {
                sqlite3_backup_step(pBackup, -1);
                sqlite3_backup_finish(pBackup);
            }
            result_code = sqlite3_errcode(pTo);
            if (result_code == SQLITE_OK)
            {
                if (save) is_game_save_exist = true;
                else is_db_init = true;
            }
        }
        sqlite3_close(pFile);
    }
    else {
        result_code = SQLITE_ERROR;
    }
    
    bool result = (result_code == SQLITE_OK);
    if (!result) throw std::runtime_error("Game save or load error.");
    
    return result;
}

bool DBManager::save ()
{
    return load_or_save(true);
}

bool DBManager::load ()
{
    return load_or_save(false);
}

long int DBManager::get_last_insert_id ()
{
    return sqlite3_last_insert_rowid(pdb);
}

template <>
std::string DBManager::get_character_value <std::string> (const std::string & value_name)
{
    Data_Rows_t result;
    select_values("character_values", "code = \"" + value_name + "\"", & result);
    return (result.size() > 0) ? result[0]["value"] : std::string{""};
}

template <>
void DBManager::set_character_value <std::string> (const std::string & value_name, const std::string & value)
{
    Row_t row;
    row["value"] = value;
    set_values("character_values", row, "code = \"" + value_name + "\"");
}

std::vector <WeaponsDBParams> DBManager::get_weapons_on_map (unsigned int map_number)
{
    std::vector <WeaponsDBParams> result;
    std::string query = "SELECT * FROM weapons_on_maps WHERE map_number = " + NumberToString(map_number);
    Data_Rows_t rows;
    sql_query_exec(query.c_str(), &rows);
    for (const auto & row : rows) result.emplace_back <WeaponsDBParams> ({
        StringToNumber <unsigned int> (row.at("id")),
        StringToNumber <unsigned int> (row.at("map_number")),
        row.at("weapon_code"),
        StringToNumber <int> (row.at("pos_x")),
        StringToNumber <int> (row.at("pos_y"))
    });
    return result;
}

std::vector <BulletsDBParams> DBManager::get_bullets_on_map (unsigned int map_number)
{
    std::vector <BulletsDBParams> result;
    std::string query = "SELECT * FROM bullets_on_maps WHERE map_number = " + NumberToString(map_number);
    Data_Rows_t rows;
    sql_query_exec(query.c_str(), &rows);
    for (const auto & row : rows) result.emplace_back <BulletsDBParams> ({
        StringToNumber <unsigned int> (row.at("id")),
        StringToNumber <unsigned int> (row.at("map_number")),
        row.at("bullet_code"),
        StringToNumber <int> (row.at("pos_x")),
        StringToNumber <int> (row.at("pos_y")),
        StringToNumber <unsigned int> (row.at("count"))
    });
    return result;
}

std::vector <SimpleObjectDBParams> DBManager::get_simple_objects_on_map (unsigned int map_number)
{
    std::vector <SimpleObjectDBParams> result;
    std::string query = "SELECT * FROM simple_objects WHERE map_number = " + NumberToString(map_number);
    Data_Rows_t rows;
    sql_query_exec(query.c_str(), &rows);
    for (const auto & row : rows) result.emplace_back <SimpleObjectDBParams> ({
        StringToNumber <unsigned int> (row.at("id")),
        StringToNumber <unsigned int> (row.at("map_number")),
        simple_object_code_to_type(row.at("type")),
        row.at("code"),
        StringToNumber <int> (row.at("pos_x")),
        StringToNumber <int> (row.at("pos_y")),
    });
    return result;
}

std::vector <EnemyDBParams> DBManager::get_enemies_on_map (unsigned int map_number)
{
    std::vector <EnemyDBParams> result;
    std::string query = "SELECT * FROM enemies WHERE map_number = " + NumberToString(map_number);
    Data_Rows_t rows;
    sql_query_exec(query.c_str(), &rows);
    for (const auto & row : rows) result.emplace_back <EnemyDBParams> ({
        StringToNumber <unsigned int> (row.at("id")),
        StringToNumber <unsigned int> (row.at("map_number")),
        row.at("type_code"),
        StringToNumber <int> (row.at("base_x")),
        StringToNumber <int> (row.at("base_y")),
        StringToNumber <int> (row.at("pos_x")),
        StringToNumber <int> (row.at("pos_y")),
        row.at("weapon1"),
        row.at("weapon2"),
        StringToNumber <unsigned int> (row.at("bullets")),
        StringToNumber <unsigned int> (row.at("strength")),
        StringToNumber <unsigned int> (row.at("accuracy")),
        StringToNumber <float> (row.at("angle")),
        StringToNumber <unsigned int> (row.at("damage"))
    });
    return result;
}

// weapons_on_maps (map_number INTEGER, weapon_code TEXT, pos_x INTEGER, pos_y INTEGER)
void DBManager::add_weapon_on_map (const WeaponsDBParams & p)
{
    std::string query = "INSERT INTO weapons_on_maps (map_number, weapon_code, pos_x, pos_y) "
                        "VALUES (" + NumberToString(p.map_number) + ", \"" + p.code + "\", "
                        + NumberToString(p.x) + ", " + NumberToString(p.y) + "); ";
    sql_query_exec(query.c_str());
}

// bullets_on_maps (map_number INTEGER, bullet_code TEXT, pos_x INTEGER, pos_y INTEGER, count INTEGER)
void DBManager::add_bullet_on_map (const BulletsDBParams & p)
{
    std::string query = "INSERT INTO bullets_on_maps (map_number, bullet_code, pos_x, pos_y, count) "
                        "VALUES (" + NumberToString(p.map_number) + ", \"" + p.code + "\", " 
                        + NumberToString(p.x) + ", " + NumberToString(p.y) + ", " + NumberToString(p.count) + "); ";
    sql_query_exec(query.c_str());
}

// simple_objects (map_number INTEGER, type TEXT, code TEXT, pos_x INTEGER, pos_y INTEGER)
void DBManager::add_simple_object_on_map (const SimpleObjectDBParams & p)
{
    std::string query = "INSERT INTO simple_objects (map_number, type, code, pos_x, pos_y) "
                        "VALUES (" + NumberToString(p.map_number) + ", \"" + simple_object_type_to_code(p.type) + "\", "
                        "\"" + p.code + "\", " + NumberToString(p.x) + ", " + NumberToString(p.y) + "); ";
    sql_query_exec(query.c_str());
}

// enemies (map_number INTEGER, type_code TEXT, base_x INTEGER, base_y INTEGER, pos_x INTEGER, pos_y INTEGER, weapon1 TEXT, weapon2 TEXT, bullets INTEGER, 
// strength INTEGER, accuracy INTEGER, angle REAL, damage INTEGER)
void DBManager::add_enemy_on_map (const EnemyDBParams & p)
{
    std::string query = "INSERT INTO enemies (map_number, type_code, base_x, base_y, pos_x, pos_y, "
                                              "weapon1, weapon2, bullets, strength, accuracy, angle, damage) "
                        "VALUES (" + NumberToString(p.map_number) + ", \"" + p.type_code + "\", " 
                        + NumberToString(p.base_x) + ", " + NumberToString(p.base_y) + ", " 
                        + NumberToString(p.x) + ", " + NumberToString(p.y) + ", \"" + 
                        p.weapon1 + "\", \"" + p.weapon2 + "\", " + NumberToString(p.bullets) + ", " 
                        + NumberToString(p.strength) + ", " + NumberToString(p.accuracy) + ", " + NumberToString(p.angle) 
                        + ", " + NumberToString(p.damage) + "); ";
    sql_query_exec(query.c_str());
}

void DBManager::delete_weapon_from_map (unsigned int id)
{
    std::string query = "DELETE FROM weapons_on_maps WHERE id = " + NumberToString(id);
    sql_query_exec(query.c_str());
}

void DBManager::delete_bullet_from_map (unsigned int id)
{
    std::string query = "DELETE FROM bullets_on_maps WHERE id = " + NumberToString(id);
    sql_query_exec(query.c_str());
}

void DBManager::delete_simple_object_from_map (unsigned int id)
{
    std::string query = "DELETE FROM simple_objects WHERE id = " + NumberToString(id);
    sql_query_exec(query.c_str());
}

void DBManager::delete_enemy_from_map (unsigned int id)
{
    std::string query = "DELETE FROM enemies WHERE id = " + NumberToString(id);
    sql_query_exec(query.c_str());
}

EnemyDBParams DBManager::get_enemy (unsigned int id)
{
    if (id > 0)
    {
        std::string query = "SELECT * FROM enemies WHERE id = " + NumberToString(id);
        Data_Rows_t rows;
        sql_query_exec(query.c_str(), &rows);
        if (rows.size() > 0)
            return EnemyDBParams {
                id,
                StringToNumber <unsigned int> (rows[0].at("map_number")),
                rows[0].at("type_code"),
                StringToNumber <int> (rows[0].at("base_x")),
                StringToNumber <int> (rows[0].at("base_y")),
                StringToNumber <int> (rows[0].at("pos_x")),
                StringToNumber <int> (rows[0].at("pos_y")),
                rows[0].at("weapon1"),
                rows[0].at("weapon2"),
                StringToNumber <unsigned int> (rows[0].at("bullets")),
                StringToNumber <unsigned int> (rows[0].at("strength")),
                StringToNumber <unsigned int> (rows[0].at("accuracy")),
                StringToNumber <float> (rows[0].at("angle")),
                StringToNumber <unsigned int> (rows[0].at("damage")),
            };
    }
    return EnemyDBParams {0, 0, "", 0, 0, 0, 0, "", "", 0, 0, 0};
}

// enemies (map_number INTEGER, type_code TEXT, base_x INTEGER, base_y INTEGER, pos_x INTEGER, pos_y INTEGER, 
// weapon1 TEXT, weapon2 TEXT, bullets INTEGER, strength INTEGER, accuracy INTEGER, angle REAL, damage INTEGER)
void DBManager::update_enemy (const EnemyDBParams & p)
{
    std::string query = "UPDATE enemies SET "
                        "map_number = " + NumberToString(p.map_number) + ", "
                        "type_code = \"" + p.type_code + "\", "
                        "base_x = " + NumberToString(p.base_x) + ", "
                        "base_y = " + NumberToString(p.base_y) + ", "
                        "pos_x = " + NumberToString(p.x) + ", "
                        "pos_y = " + NumberToString(p.y) + ", "
                        "weapon1 = \"" + p.weapon1 + "\", "
                        "weapon2 = \"" + p.weapon2 + "\", "
                        "bullets = " +  NumberToString(p.bullets) + ", "
                        "strength = " + NumberToString(p.strength) + ", "
                        "accuracy = " + NumberToString(p.accuracy) + ", "
                        "angle = " + NumberToString(p.angle) + ", "
                        "damage = " + NumberToString(p.damage) + " "
                        "WHERE id = " + NumberToString(p.id);
    sql_query_exec(query.c_str());
}

void DBManager::update_enemy_field (unsigned int id, const std::string & field_name, const std::string & value, bool need_quotes)
{
    std::string query = "UPDATE enemies SET " + field_name + " = ";
    if (need_quotes) query += "\"";
    query += value;
    if (need_quotes) query += "\"";
    query += " WHERE id = " + NumberToString(id);
    sql_query_exec(query.c_str());
}
