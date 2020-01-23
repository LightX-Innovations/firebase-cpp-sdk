#ifndef FIREBASE_FIRESTORE_CLIENT_CPP_SRC_IOS_FIRESTORE_IOS_H_
#define FIREBASE_FIRESTORE_CLIENT_CPP_SRC_IOS_FIRESTORE_IOS_H_

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>  // NOLINT(build/c++11)
#include <unordered_set>

#include "app/src/cleanup_notifier.h"
#include "app/src/future_manager.h"
#include "app/src/include/firebase/app.h"
#include "firestore/src/include/firebase/firestore/collection_reference.h"
#include "firestore/src/include/firebase/firestore/document_reference.h"
#include "firestore/src/include/firebase/firestore/settings.h"
#include "firestore/src/ios/promise_factory_ios.h"
#include "firebase-ios-sdk/Firestore/core/src/firebase/firestore/api/firestore.h"
#include "firebase-ios-sdk/Firestore/core/src/firebase/firestore/auth/credentials_provider.h"
#include "firebase-ios-sdk/Firestore/core/src/firebase/firestore/model/database_id.h"

namespace firebase {
namespace firestore {

class ListenerRegistrationInternal;
class Transaction;
class TransactionFunction;
class WriteBatch;

class FirestoreInternal {
 public:
  explicit FirestoreInternal(App* app);
  ~FirestoreInternal();

  FirestoreInternal(const FirestoreInternal&) = delete;
  FirestoreInternal& operator=(const FirestoreInternal&) = delete;

  App* app() const { return app_; }

  // Whether this object was successfully initialized by the constructor.
  bool initialized() const { return app_ != nullptr; }

  // Manages all Future objects returned from Firestore API.
  FutureManager& future_manager() { return future_manager_; }

  // When this is deleted, it will clean up all DatabaseReferences,
  // DataSnapshots, and other such objects.
  CleanupNotifier& cleanup() { return cleanup_; }

  CollectionReference Collection(const char* collection_path) const;

  DocumentReference Document(const char* document_path) const;

  Query CollectionGroup(const char* collection_id) const;

  Settings settings() const;
  void set_settings(const Settings& settings);

  WriteBatch batch() const;

  Future<void> RunTransaction(
      std::function<Error(Transaction*, std::string*)> update);
  Future<void> RunTransaction(TransactionFunction* update);
  Future<void> RunTransactionLastResult();

  Future<void> DisableNetwork();
  Future<void> DisableNetworkLastResult();

  Future<void> EnableNetwork();
  Future<void> EnableNetworkLastResult();

  Future<void> Terminate();
  Future<void> TerminateLastResult();

  Future<void> WaitForPendingWrites();
  Future<void> WaitForPendingWritesLastResult();

  Future<void> ClearPersistence();
  Future<void> ClearPersistenceLastResult();

  const model::DatabaseId& database_id() const {
    return firestore_->database_id();
  }

  // Manages the ListenerRegistrationInternal objects.
  void RegisterListenerRegistration(ListenerRegistrationInternal* registration);
  void UnregisterListenerRegistration(
      ListenerRegistrationInternal* registration);

  // TODO(varconst): this method should become unnecessary once
  // `api::Transaction::Lookup()` starts returning C++ documents.
  const std::shared_ptr<api::Firestore>& firestore_core() const {
    return firestore_;
  }

 private:
  friend class TestFriend;

  enum class AsyncApi {
    kEnableNetwork = 0,
    kDisableNetwork,
    kRunTransaction,
    kTerminate,
    kWaitForPendingWrites,
    kClearPersistence,
    kCount,
  };

  FirestoreInternal(App* app,
                    std::unique_ptr<auth::CredentialsProvider> credentials);

  std::shared_ptr<api::Firestore> CreateFirestore(
      App* app, std::unique_ptr<auth::CredentialsProvider> credentials);

  // Gets the reference-counted Future implementation of this instance, which
  // can be used to create a Future.
  ReferenceCountedFutureImpl* ref_future() {
    return future_manager_.GetFutureApi(this);
  }

  void ClearListeners();

  void ApplyDefaultSettings();

  App* app_ = nullptr;
  std::shared_ptr<api::Firestore> firestore_;

  CleanupNotifier cleanup_;

  FutureManager future_manager_;
  PromiseFactory<AsyncApi> promise_factory_{&future_manager_};

  // TODO(b/136119216): revamp this mechanism on both iOS and Android.
  std::mutex listeners_mutex_;
  std::unordered_set<ListenerRegistrationInternal*> listeners_;
};

}  // namespace firestore
}  // namespace firebase

#endif  // FIREBASE_FIRESTORE_CLIENT_CPP_SRC_IOS_FIRESTORE_IOS_H_