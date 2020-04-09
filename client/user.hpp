#ifndef USER_HPP
#define USER_HPP
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QVector>
#include <QImage>

/**
 * @brief The User class
 * the main purpose of this class is to store all fingers belonging to one user
 * and its serialization into byte array, so it can be easily sent trough socket
 */
class User
{
private:
    quint8 fingers_count;
    QVector<QImage> stored_fingers;

public:
    User();
    User(const quint8& fingers_count, const QVector<QImage>& stored_fingers);
    User(const User& other);

    // below functions return a number of fingerprints that will be stored in user after a function ends
    quint8 addFinger(const QImage& fp);
    quint8 removeFinger(const quint8 index);
    quint8 removeAllFingers();
    const quint8& getFingersCount();

    // below functions return no type
    void serializeUser(QByteArray& outBuffer, int operation);
    void deserializeUser(const QByteArray& inBuffer, User* newUser);

    // below functions return misc types
    QImage getFingerOnIndex(const quint8 index); // returns empty vector if index is not valid
};

#endif // USER_HPP
