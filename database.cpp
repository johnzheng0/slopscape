#include "database.h"


Database::Database()
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QFile dbFile(dataDir + "/app.db");
    sqlite3_open(dbFile.fileName().toStdString().c_str(), &db);
}

Database::~Database()
{
    sqlite3_close(db);
}

void Database::initTables()
{
    // create tables if they dont exist
    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS files ("
                 "id INTEGER PRIMARY KEY,"
                 "location TEXT UNIQUE);",
                 nullptr, nullptr, nullptr);
    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS tags ("
                 "id INTEGER PRIMARY KEY,"
                 "name TEXT UNIQUE);",
                 nullptr, nullptr, nullptr);
    sqlite3_exec(db,
                 "CREATE TABLE IF NOT EXISTS files_tags ("
                 "file_id INTEGER,"
                 "tag_id INTEGER,"
                 "FOREIGN KEY (file_id) REFERENCES files(id) ON DELETE CASCADE,"
                 "FOREIGN KEY (tag_id) REFERENCES tags(id) ON DELETE CASCADE,"
                 "PRIMARY KEY (file_id, tag_id));",
                 nullptr, nullptr, nullptr);
    sqlite3_exec(db,
                 "INSERT INTO tags (name)"
                 "VALUES ('image');",
                 nullptr, nullptr, nullptr);
    sqlite3_exec(db,
                 "INSERT INTO tags (name)"
                 "VALUES ('gif');",
                 nullptr, nullptr, nullptr);
    sqlite3_exec(db,
                 "INSERT INTO tags (name)"
                 "VALUES ('video');",
                 nullptr, nullptr, nullptr);
}

void Database::insertFiles(QStringList locations)
{
    // sanitize
    locations = sanitizeQStringList(locations);

    // initialize sql and statement
    std::string sql;
    sqlite3_stmt *stmt;

    // create query and execute to create new file entries
    sql = "INSERT OR IGNORE INTO files (location) VALUES ('" + locations.join("'),('").toStdString() + "');";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // sort locations into
    QStringList imageLocations;
    QStringList gifLocations;
    QStringList videoLocations;
    for (auto& location : locations) {
        QFileInfo fileInfo(location);
        QString extension = fileInfo.suffix().toLower();
        if (extension == "jpg" || extension == "jpeg" || extension == "png"|| extension == "bmp") {
            imageLocations.push_back(location);
        } else if (extension == "mp4" || extension == "webm") {
            videoLocations.push_back(location);
        } else if (extension == "gif" ) {
            gifLocations.push_back(location);
        }
    }

    // create query and execute to assing media tags
    sql =
        "INSERT OR IGNORE INTO files_tags (file_id, tag_id) "
        "SELECT files.id, 1 FROM files "
        "WHERE files.location IN (('" + imageLocations.join("'),('").toStdString() + "')) "
        "UNION SELECT files.id, 2 FROM files "
        "WHERE files.location IN (('" + gifLocations.join("'),('").toStdString() + "')) "
        "UNION SELECT files.id, 3 FROM files "
        "WHERE files.location IN (('" + videoLocations.join("'),('").toStdString() + "'));";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Database::insertTags(QStringList names)
{
    // create query and execute
    std::string sql = "INSERT OR IGNORE INTO tags (name) VALUES ('" + names.join("'),('").toStdString() + "');";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Database::insertFile(QString location)
{
    // sanitize
    location = sanitizeQString(location);

    // create query and execute
    std::string sql = "INSERT OR IGNORE INTO files (location) VALUES ('" + location.toStdString() + "');";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Database::insertTag(QString name)
{
    // create query and execute
    std::string sql = "INSERT OR IGNORE INTO tags (name) VALUES ('" + name.toStdString() + "');";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

FileRecord Database::getFile(QString location)
{
    // sanitize
    location = sanitizeQString(location);

    // give empty file if empty location
    if (location.isEmpty()) return FileRecord(0, "");

    // create query and execute
    std::string sql = "SELECT * FROM files WHERE location='" + location.toStdString() + "' LIMIT 1;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        // failure gives fake file
        sqlite3_finalize(stmt);
        return FileRecord(0, location);
    }
    size_t get_id  = std::strtoul(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), nullptr, 10);
    QString get_location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

    sqlite3_finalize(stmt);
    return FileRecord(get_id, get_location);
}

FileRecord Database::getFile(size_t id)
{
    // create query and execute
    std::string sql = "SELECT * FROM files WHERE id=" + std::to_string(id) + " LIMIT 1;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        // failure gives empty fake file
        sqlite3_finalize(stmt);
        return FileRecord(0, "");
    }

    // process results
    size_t get_id  = std::strtoul(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), nullptr, 10);
    QString get_location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    sqlite3_finalize(stmt);
    return FileRecord(get_id, get_location);
}

QVector<FileRecord> Database::getAllFiles()
{
    // execute query
    std::string sql = "SELECT * FROM files;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    // process results
    QVector<FileRecord> records;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        size_t get_id = std::strtoul(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), nullptr, 10);
        QString get_location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        records.push_back(FileRecord(get_id, get_location));
    }
    return records;
}

TagRecord Database::getTag(QString name)
{
    // empty name gives empty fake tag
    if (name.isEmpty()) return TagRecord(0, "");

    // create query and execute
    std::string sql = "SELECT * FROM tags WHERE name='" + name.toStdString() + "' LIMIT 1;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        // failure gives fake tag
        sqlite3_finalize(stmt);
        return TagRecord(0, name);
    }

    // process results
    size_t get_id = std::strtoul(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), nullptr, 10);
    QString get_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    sqlite3_finalize(stmt);
    return TagRecord(get_id, get_name);
}

TagRecord Database::getOrCreateTag(QString name)
{
    // empty name gives empty tag
    if (name.isEmpty()) return TagRecord(0, "");

    // create query and execute
    std::string sql = "SELECT * FROM tags WHERE name='" + name.toStdString() + "' LIMIT 1;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        // failure creates a new tag and returns it
        sqlite3_finalize(stmt);
        insertTag(name);
        return getTag(name);
    }

    // process results
    size_t get_id = std::strtoul(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), nullptr, 10);
    QString get_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    sqlite3_finalize(stmt);
    return TagRecord(get_id, get_name);
}

QVector<TagRecord> Database::getAllTags()
{
    // execute query
    std::string sql = "SELECT * FROM tags;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    // process results
    QVector<TagRecord> records;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        size_t get_id = std::strtoul(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), nullptr, 10);
        QString get_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        records.push_back(TagRecord(get_id, get_name));
    }
    sqlite3_finalize(stmt);
    return records;
}

TagRecord Database::getTag(size_t id)
{
    // create query and execute
    std::string sql = "SELECT * FROM tags WHERE id=" + std::to_string(id) + " LIMIT 1;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (sqlite3_step(stmt) != SQLITE_ROW) {
        // failure gives empty fake tag
        sqlite3_finalize(stmt);
        return TagRecord(0, "");
    }

    // process results
    size_t get_id = std::strtoul(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), nullptr, 10);
    QString get_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    sqlite3_finalize(stmt);
    return TagRecord(get_id, get_name);
}

QVector<FileRecord> Database::getFilesFromTags(QVector<TagRecord> tags)
{
    // create condition statement
    std::string condition;
    if (!tags.empty()) {
        condition = "WHERE tag_id IN (";
        condition.append(std::to_string(tags.front().getId()));
        for (size_t i=1; i<tags.size(); i++) {
            condition.append("," + std::to_string(tags[i].getId()));
        }
        condition.append(")");
    }

    // count duplicates in tags
    size_t duplicateCount = 0;
    for (int i=1; i<tags.size(); i++) {
        for (int j=i; j>0; j--) {
            if (tags[j].getId() == tags[j-1].getId()) {
                duplicateCount++;
                break;
            }
            std::swap(tags[j], tags[j-1]);
        }
    }

    // execute query
    std::string sql = "SELECT files.* FROM files JOIN files_tags ON id=file_id " + condition + " GROUP BY file_id HAVING COUNT(DISTINCT tag_id)=" + std::to_string(tags.size()-duplicateCount) + ";";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    // process results
    QVector<FileRecord> records;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        size_t get_id = std::strtoul(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), nullptr, 10);
        QString get_location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        records.push_back(FileRecord(get_id, get_location));
    }
    sqlite3_finalize(stmt);
    return records;
}

void Database::attachTagsToFiles(QVector<FileRecord> files, QVector<TagRecord> tags)
{
    // create query and execute
    QStringList entryList;
    for (auto& file : files) {
        for (auto tag : tags) {
            if (!tag.exist()) tag = getOrCreateTag(tag.getName());
            if (tag.getId() <= 3) continue; // ignore media type tags
            entryList.push_back(QString("(%1,%2)").arg(file.getId()).arg(tag.getId()));
        }
    }
    std::string sql = "INSERT OR IGNORE INTO files_tags (file_id,tag_id) VALUES " + entryList.join(",").toStdString() + ";";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Database::removeTagsFromFiles(QVector<FileRecord> files, QVector<TagRecord> tags)
{
    // create query and execute
    QStringList fileList;
    for (auto& file : files) {
        fileList.push_back(QString::number(file.getId()));
    }
    QStringList tagList;
    for (auto& tag : tags) {
        TagRecord realTag = getTag(tag.getName());
        if (realTag.getId() > 3) tagList.push_back(QString::number(realTag.getId()));
    }

    std::string sql = "DELETE FROM files_tags WHERE file_id IN (" + fileList.join(",").toStdString() + ") AND tag_id IN (" + tagList.join(",").toStdString() + ");";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Database::removeAllTagsFromFiles(QVector<FileRecord> files)
{
    // create query and execute
    QStringList fileList;
    for (auto& file : files) {
        fileList.push_back(QString::number(file.getId()));
    }

    std::string sql = "DELETE FROM files_tags WHERE file_id IN (" + fileList.join(",").toStdString() + ") AND tag_id NOT IN (1,2,3);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

QVector<FileRecord> Database::getFilesWithoutTags()
{
    // execute query
    std::string sql = "SELECT files.* FROM files LEFT JOIN "
                      "(SELECT DISTINCT file_id FROM files_tags WHERE tag_id NOT IN (1,2,3)) "
                      "AS blacklist ON files.id = blacklist.file_id "
                      "WHERE blacklist.file_id is NULL ORDER BY id DESC;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    // process results
    QVector<FileRecord> records;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        size_t get_id = std::strtoul(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), nullptr, 10);
        QString get_location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        records.push_back(FileRecord(get_id, get_location));
    }
    sqlite3_finalize(stmt);
    return records;
}

QVector<FileRecord> Database::getFilesWithTags()
{
    // execute query
    std::string sql = "SELECT files.* FROM files LEFT JOIN "
                      "(SELECT DISTINCT file_id FROM files_tags WHERE tag_id NOT IN (1,2,3)) "
                      "AS blacklist ON files.id = blacklist.file_id "
                      "WHERE blacklist.file_id is NOT NULL ORDER BY id DESC;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    // process results
    QVector<FileRecord> records;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        size_t get_id = std::strtoul(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), nullptr, 10);
        QString get_location = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        records.push_back(FileRecord(get_id, get_location));
    }
    sqlite3_finalize(stmt);
    return records;
}

QVector<TagRecord> Database::getFileTags(FileRecord file)
{
    QVector<TagRecord> results;
    if (!file.exist()) return results; // fake file gives no tags

    // create query and execute
    std::string sql = "SELECT tag_id FROM files_tags WHERE file_id=" + std::to_string(file.getId()) + ";";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    // process results
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        size_t get_id = std::strtoul(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)), nullptr, 10);
        results.push_back(getTag(get_id));
    }
    sqlite3_finalize(stmt);
    return results;
}

QStringList Database::getAllTagsNames()
{
    // create query and execute
    std::string sql = "SELECT name FROM tags;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    // process results
    QStringList results;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        QString tag = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        results.push_back(tag);
    }
    sqlite3_finalize(stmt);
    return results;
}


QStringList Database::getSimilarTags(QString tag_name, size_t limit)
{
    QStringList results;
    // if (tag_name.isEmpty()) return results;

    std::string sql = "SELECT name FROM tags WHERE name LIKE '" + tag_name.toStdString() + "%' LIMIT " + std::to_string(limit) + ";";
    sqlite3_stmt *stmt;

    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // Accessing each column of the current row
        QString tag = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        results.push_back(tag);
    }
    sqlite3_finalize(stmt);
    return results;
}

void Database::deleteUnusedTags()
{
    // make query and execute
    std::string sql = "DELETE FROM tags WHERE NOT EXISTS (SELECT 1 FROM files_tags WHERE files_tags.tag_id = tags.id) AND tags.id NOT IN (1,2,3);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Database::deleteUnusedJunctions() {
    std::string sql = "DELETE FROM files_tags WHERE NOT EXISTS (SELECT 1 FROM files WHERE files_tags.file_id = files.id);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Database::deleteFiles(QVector<FileRecord> files)
{
    // construct condition statement
    std::string condition;
    if (!files.empty()) {
        condition = "(";
        condition.append(std::to_string(files.front().getId()));
        for (int i=1; i<files.size(); i++) {
            condition.append("," + std::to_string(files[i].getId()));
        }
        condition.append(")");
    }

    // initialize query and statement
    std::string sql;
    sqlite3_stmt *stmt;

    // make query and execute deletion of files
    sql = "DELETE FROM files WHERE id IN " + condition + ";";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // make query and execute deletion of many-to-many relations
    sql = "DELETE FROM files_tags WHERE file_id IN " + condition + ";";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Database::deleteTags(QVector<TagRecord> tags)
{
    // construct condition statement
    std::string condition;
    if (!tags.empty()) {
        condition = "(";
        condition.append(std::to_string(tags.front().getId()));
        for (int i=1; i<tags.size(); i++) {
            condition.append("," + std::to_string(tags[i].getId()));
        }
        condition.append(")");
    }

    // initialize query and statement
    std::string sql;
    sqlite3_stmt *stmt;

    // make query and execute deletion of files
    sql = "DELETE FROM tags WHERE id IN " + condition + ";";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    // make query and execute deletion of many-to-many relations
    sql = "DELETE FROM files_tags WHERE tag_id IN " + condition + ";";
    sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

void Database::deleteFile(FileRecord file)
{
    // ignore fake files
    if (!file.exist()) return;
    deleteFiles({file});
}

void Database::deleteTag(TagRecord tag)
{
    // ignore fake tags
    if (!tag.exist()) return;
    deleteTags({tag});
}

QStringList Database::sanitizeQStringList(const QStringList &inputList)
{
    QStringList sanitizedList;
    for (const QString& input : inputList) {
        QString sanitized = input;
        sanitizedList.push_back(sanitized.replace("'", "''"));
    }
    return sanitizedList;
}

QString Database::sanitizeQString(const QString &input)
{
    QString output = input;
    return output.replace("'", "''");
}
