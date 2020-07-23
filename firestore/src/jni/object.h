#ifndef FIREBASE_FIRESTORE_CLIENT_CPP_SRC_JNI_OBJECT_H_
#define FIREBASE_FIRESTORE_CLIENT_CPP_SRC_JNI_OBJECT_H_

#include <jni.h>

#include <string>

namespace firebase {
namespace firestore {
namespace jni {

class Env;

/**
 * A wrapper for a JNI `jobject` that adds additional behavior.
 *
 * `Object` merely holds values with `jobject` types, see `Local` and `Global`
 * template subclasses for reference-type-aware wrappers that automatically
 * manage the lifetime of JNI objects.
 */
class Object {
 public:
  Object() = default;
  explicit Object(jobject object) : object_(object) {}
  virtual ~Object() = default;

  explicit operator bool() const { return object_ != nullptr; }

  virtual jobject get() const { return object_; }

  /**
   * Converts this object to a C++ String by calling the Java `toString` method
   * on it.
   */
  std::string ToString(JNIEnv* env) const;
  std::string ToString(Env& env) const;

 protected:
  jobject object_ = nullptr;
};

inline bool operator==(const Object& lhs, const Object& rhs) {
  return lhs.get() == rhs.get();
}

inline bool operator!=(const Object& lhs, const Object& rhs) {
  return !(lhs == rhs);
}

}  // namespace jni
}  // namespace firestore
}  // namespace firebase

#endif  // FIREBASE_FIRESTORE_CLIENT_CPP_SRC_JNI_OBJECT_H_
