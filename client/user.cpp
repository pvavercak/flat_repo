#include "user.hpp"

User::User() : fingers_count{0}, stored_fingers(0) {} // default

User::User(const quint8 &fingers_count, const fingers &stored_fingers)
{
    this->fingers_count = fingers_count;
    this->stored_fingers = stored_fingers;
}

User::User(const User &other)
{
    this->fingers_count = other.fingers_count;
    this->stored_fingers = other.stored_fingers;
}

quint8 User::addFinger(const finger &fp)
{
    stored_fingers.push_back(fp);
    return fingers_count = static_cast<quint8> (stored_fingers.size());
}

quint8 User::removeFinger(const quint8 index)
{
    stored_fingers.remove(index);
    return fingers_count = static_cast<quint8> (stored_fingers.size());
}

quint8 User::removeAllFingers()
{
    stored_fingers.erase(stored_fingers.begin(), stored_fingers.end());
    return fingers_count = static_cast<quint8> (stored_fingers.size());
}

const quint8 &User::getFingersCount()
{
    return this->fingers_count;
}

void User::serializeUser(QByteArray &outBuffer)
{
    QDataStream serialization_stream(&outBuffer, QIODevice::WriteOnly);
    serialization_stream.setVersion(QDataStream::Qt_5_9);
    serialization_stream << int(0);
    serialization_stream << fingers_count << stored_fingers;
    serialization_stream.device()->seek(0);
    serialization_stream << outBuffer.size();
}

void User::deserializeUser(const QByteArray &inBuffer, User *newUser)
{
    if (!newUser) return;
    QDataStream deserialization_stream(inBuffer);
    quint8 fpcount{0};
    fingers fps{0};
    deserialization_stream >> fpcount >> fps;
    newUser->fingers_count = fpcount;
    newUser->stored_fingers = fps;
}

