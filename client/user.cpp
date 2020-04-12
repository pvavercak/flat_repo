#include "user.hpp"

User::User() : fingers_count{0}, stored_fingers(0) {} // default

User::User(const quint8 &fingers_count, const QVector<QImage> &stored_fingers)
{
  this->fingers_count = fingers_count;
  this->stored_fingers = stored_fingers;
}

User::User(const User &other)
{
  this->fingers_count = other.fingers_count;
  this->stored_fingers = other.stored_fingers;
}

User::User(User *other)
{
  this->fingers_count = other->getFingersCount();
  this->stored_fingers = other->getStoredFingers();
}

quint8 User::addFinger(const QImage &fp)
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

const QVector<QImage>& User::getStoredFingers()
{
  return this->stored_fingers;
}

/**
 * @brief User::serializeUser
 * @param outBuffer
 * @param operation 0 means registration, 1 means identification
 */
void User::serializeUser(QByteArray &outBuffer, int operation)
{
  QDataStream serialization_stream(&outBuffer, QIODevice::WriteOnly);
  serialization_stream.setVersion(QDataStream::Qt_5_9);
  serialization_stream << int(0) << operation;
  serialization_stream << fingers_count << stored_fingers;
  serialization_stream.device()->seek(0);
  serialization_stream << outBuffer.size();
}
