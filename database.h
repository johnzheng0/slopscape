#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <QVector>
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QStandardPaths>

#include <fstream>

// generic information record superclass
class Record {
protected:
    Record(size_t idIn)
        : id(idIn) {}
public:
    Record()
        :id(0) {};
    size_t getId() const {return id;}
    bool exist() {return id != 0;}
    bool operator==(const Record& other) const {
        return id == other.id;
    }
private:
    size_t id;
};

// represents information from the files table
class FileRecord : public Record {
protected:
    FileRecord(size_t idIn, QString locationIn)
        : Record(idIn), location(locationIn) {}
public:
    FileRecord(QString locationIn)
        : Record(0), location(locationIn) {}
    const QString getLocation() const {return location;}
    bool operator==(const FileRecord& other) const {
        return location == other.location && this->getId() == other.getId();
    }
private:
    QString location;
    friend class Database;
};

// represents information from the tags table
class TagRecord : public Record {
protected:
    TagRecord(size_t idIn, QString nameIn)
        : Record(idIn), name(nameIn) {}
public:
    TagRecord(QString nameIn)
        : Record(0), name(nameIn) {}
    const QString getName() const {return name;}
    bool operator==(const TagRecord& other) const {
        return name == other.name && this->getId() == other.getId();
    }
private:
    QString name;
    friend class Database;
};


// used to interact with database
class Database
{
public:
    enum MediaType {Image=1, Gif, Video};

    Database();
    ~Database();
    void initTables();

    // creation of records
    void insertFiles(QStringList locations);
    void insertTags(QStringList names);
    void insertFile(QString location);
    void insertTag(QString name);

    // file record getters
    FileRecord getFile(QString location);
    FileRecord getFile(size_t id);
    QVector<FileRecord> getAllFiles();

    // tag record getters
    TagRecord getTag(QString name);
    TagRecord getTag(size_t id);
    TagRecord getOrCreateTag(QString name); // can create a tag if none exists
    QVector<TagRecord> getAllTags();
    QVector<TagRecord> getRealTags(QStringList names);
    QStringList getAllTagsNames(); // returns a string list

    // deletion of records (will handle deletion of many-to-many relationships)
    void deleteFiles(QVector<FileRecord> files);
    void deleteTags(QVector<TagRecord> tags);
    void deleteFile(FileRecord file);
    void deleteTag(TagRecord tag);

    // tag management
    void attachTagsToFiles(QVector<FileRecord> files, QVector<TagRecord> tags);
    void removeTagsFromFiles(QVector<FileRecord> files, QVector<TagRecord> tags);
    void removeAllTagsFromFiles(QVector<FileRecord> files);
    QVector<TagRecord> getFileTags(FileRecord file);
    QVector<FileRecord> getFilesFromTags(QVector<TagRecord> tags);
    QVector<FileRecord> getFilesWithoutTags();
    QVector<FileRecord> getFilesWithTags();
    void deleteUnusedTags();
    void deleteUnusedJunctions();

    // unused methods lol
    QVector<TagRecord> getFiles(QVector<size_t> ids, QVector<std::string> locations);
    QVector<TagRecord> getTags(QVector<size_t> ids, QVector<std::string> names);
    QStringList getSimilarTags(QString tag_name, size_t limit);
private:
    sqlite3* db;

    QStringList sanitizeQStringList(const QStringList& inputList);
    QString sanitizeQString(const QString& input);
};

#endif // DATABASE_H
