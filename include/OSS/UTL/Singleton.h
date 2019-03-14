#ifndef OSS_SINGLETON_H_INCLUDED
#define OSS_SINGLETON_H_INCLUDED

namespace OSS {
namespace UTL {

template <typename T>
class Singleton
{
public:
  static T* instance();
  static void deleteInstance();
private:
  static T* _instance;
};

template <typename T>
T* Singleton<T>::_instance = 0;

template <typename T>
inline T* Singleton<T>::instance()
{
  if (!_instance) {
    _instance = new T();
  }
  return _instance;
}

template <typename T>
inline void Singleton<T>::deleteInstance()
{
  delete _instance;
  _instance = 0;
}

} } // OSS::UTL

#endif // OSS_SINGLETON_H_INCLUDED

