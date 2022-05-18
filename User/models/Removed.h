/**
 *
 *  Removed.h
 *  DO NOT EDIT. This file is generated by drogon_ctl
 *
 */

#pragma once
#include <drogon/orm/Result.h>
#include <drogon/orm/Row.h>
#include <drogon/orm/Field.h>
#include <drogon/orm/SqlBinder.h>
#include <drogon/orm/Mapper.h>
#ifdef __cpp_impl_coroutine
#include <drogon/orm/CoroMapper.h>
#endif
#include <trantor/utils/Date.h>
#include <trantor/utils/Logger.h>
#include <json/json.h>
#include <string>
#include <memory>
#include <vector>
#include <tuple>
#include <stdint.h>
#include <iostream>

namespace drogon {
    namespace orm {
        class DbClient;

        using DbClientPtr = std::shared_ptr<DbClient>;
    }
}
namespace drogon_model {
    namespace techluster {

        class Removed {
        public:
            struct Cols {
                static const std::string _id;
                static const std::string _username;
                static const std::string _motto;
                static const std::string _region;
                static const std::string _avatar;
                static const std::string _avatar_hash;
                static const std::string _avatar_frame;
                static const std::string _clan;
                static const std::string _permission;
                static const std::string _password;
                static const std::string _email;
                static const std::string _data;
                static const std::string _timestamp;
                static const std::string _recoverable;
            };

            const static int primaryKeyNumber;
            const static std::string tableName;
            const static bool hasPrimaryKey;
            const static std::string primaryKeyName;
            using PrimaryKeyType = int64_t;

            const PrimaryKeyType &getPrimaryKey() const;

            /**
             * @brief constructor
             * @param r One row of records in the SQL query result.
             * @param indexOffset Set the offset to -1 to access all columns by column names,
             * otherwise access all columns by offsets.
             * @note If the SQL is not a style of 'select * from table_name ...' (select all
             * columns by an asterisk), please set the offset to -1.
             */
            explicit Removed(const drogon::orm::Row &r, const ssize_t indexOffset = 0) noexcept;

            /**
             * @brief constructor
             * @param pJson The json object to construct a new instance.
             */
            explicit Removed(const Json::Value &pJson) noexcept(false);

            /**
             * @brief constructor
             * @param pJson The json object to construct a new instance.
             * @param pMasqueradingVector The aliases of table columns.
             */
            Removed(const Json::Value &pJson, const std::vector<std::string> &pMasqueradingVector) noexcept(false);

            Removed() = default;

            void updateByJson(const Json::Value &pJson) noexcept(false);

            void updateByMasqueradedJson(const Json::Value &pJson,
                                         const std::vector<std::string> &pMasqueradingVector) noexcept(false);

            static bool validateJsonForCreation(const Json::Value &pJson, std::string &err);

            static bool validateMasqueradedJsonForCreation(const Json::Value &,
                                                           const std::vector<std::string> &pMasqueradingVector,
                                                           std::string &err);

            static bool validateJsonForUpdate(const Json::Value &pJson, std::string &err);

            static bool validateMasqueradedJsonForUpdate(const Json::Value &,
                                                         const std::vector<std::string> &pMasqueradingVector,
                                                         std::string &err);

            static bool validJsonOfField(size_t index,
                                         const std::string &fieldName,
                                         const Json::Value &pJson,
                                         std::string &err,
                                         bool isForCreation);

            /**  For column id  */
            ///Get the value of the column id, returns the default value if the column is null
            const int64_t &getValueOfId() const noexcept;

            ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
            const std::shared_ptr<int64_t> &getId() const noexcept;

            ///Set the value of the column id
            void setId(const int64_t &pId) noexcept;

            /**  For column username  */
            ///Get the value of the column username, returns the default value if the column is null
            const std::string &getValueOfUsername() const noexcept;

            ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
            const std::shared_ptr<std::string> &getUsername() const noexcept;

            ///Set the value of the column username
            void setUsername(const std::string &pUsername) noexcept;

            void setUsername(std::string &&pUsername) noexcept;

            void setUsernameToNull() noexcept;

            /**  For column motto  */
            ///Get the value of the column motto, returns the default value if the column is null
            const std::string &getValueOfMotto() const noexcept;

            ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
            const std::shared_ptr<std::string> &getMotto() const noexcept;

            ///Set the value of the column motto
            void setMotto(const std::string &pMotto) noexcept;

            void setMotto(std::string &&pMotto) noexcept;

            void setMottoToNull() noexcept;

            /**  For column region  */
            ///Get the value of the column region, returns the default value if the column is null
            const int32_t &getValueOfRegion() const noexcept;

            ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
            const std::shared_ptr<int32_t> &getRegion() const noexcept;

            ///Set the value of the column region
            void setRegion(const int32_t &pRegion) noexcept;

            void setRegionToNull() noexcept;

            /**  For column avatar  */
            ///Get the value of the column avatar, returns the default value if the column is null
            const std::string &getValueOfAvatar() const noexcept;

            ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
            const std::shared_ptr<std::string> &getAvatar() const noexcept;

            ///Set the value of the column avatar
            void setAvatar(const std::string &pAvatar) noexcept;

            void setAvatar(std::string &&pAvatar) noexcept;

            void setAvatarToNull() noexcept;

            /**  For column avatar_hash  */
            ///Get the value of the column avatar_hash, returns the default value if the column is null
            const std::string &getValueOfAvatarHash() const noexcept;

            ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
            const std::shared_ptr<std::string> &getAvatarHash() const noexcept;

            ///Set the value of the column avatar_hash
            void setAvatarHash(const std::string &pAvatarHash) noexcept;

            void setAvatarHash(std::string &&pAvatarHash) noexcept;

            void setAvatarHashToNull() noexcept;

            /**  For column avatar_frame  */
            ///Get the value of the column avatar_frame, returns the default value if the column is null
            const int32_t &getValueOfAvatarFrame() const noexcept;

            ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
            const std::shared_ptr<int32_t> &getAvatarFrame() const noexcept;

            ///Set the value of the column avatar_frame
            void setAvatarFrame(const int32_t &pAvatarFrame) noexcept;

            void setAvatarFrameToNull() noexcept;

            /**  For column clan  */
            ///Get the value of the column clan, returns the default value if the column is null
            const std::string &getValueOfClan() const noexcept;

            ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
            const std::shared_ptr<std::string> &getClan() const noexcept;

            ///Set the value of the column clan
            void setClan(const std::string &pClan) noexcept;

            void setClan(std::string &&pClan) noexcept;

            void setClanToNull() noexcept;

            /**  For column permission  */
            ///Get the value of the column permission, returns the default value if the column is null
            const std::string &getValueOfPermission() const noexcept;

            ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
            const std::shared_ptr<std::string> &getPermission() const noexcept;

            ///Set the value of the column permission
            void setPermission(const std::string &pPermission) noexcept;

            void setPermission(std::string &&pPermission) noexcept;

            void setPermissionToNull() noexcept;

            /**  For column password  */
            ///Get the value of the column password, returns the default value if the column is null
            const std::string &getValueOfPassword() const noexcept;

            ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
            const std::shared_ptr<std::string> &getPassword() const noexcept;

            ///Set the value of the column password
            void setPassword(const std::string &pPassword) noexcept;

            void setPassword(std::string &&pPassword) noexcept;

            void setPasswordToNull() noexcept;

            /**  For column email  */
            ///Get the value of the column email, returns the default value if the column is null
            const std::string &getValueOfEmail() const noexcept;

            ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
            const std::shared_ptr<std::string> &getEmail() const noexcept;

            ///Set the value of the column email
            void setEmail(const std::string &pEmail) noexcept;

            void setEmail(std::string &&pEmail) noexcept;

            void setEmailToNull() noexcept;

            /**  For column data  */
            ///Get the value of the column data, returns the default value if the column is null
            const std::string &getValueOfData() const noexcept;

            ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
            const std::shared_ptr<std::string> &getData() const noexcept;

            ///Set the value of the column data
            void setData(const std::string &pData) noexcept;

            void setData(std::string &&pData) noexcept;

            /**  For column timestamp  */
            ///Get the value of the column timestamp, returns the default value if the column is null
            const ::trantor::Date &getValueOfTimestamp() const noexcept;

            ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
            const std::shared_ptr<::trantor::Date> &getTimestamp() const noexcept;

            ///Set the value of the column timestamp
            void setTimestamp(const ::trantor::Date &pTimestamp) noexcept;

            /**  For column recoverable  */
            ///Get the value of the column recoverable, returns the default value if the column is null
            const bool &getValueOfRecoverable() const noexcept;

            ///Return a shared_ptr object pointing to the column const value, or an empty shared_ptr object if the column is null
            const std::shared_ptr<bool> &getRecoverable() const noexcept;

            ///Set the value of the column recoverable
            void setRecoverable(const bool &pRecoverable) noexcept;


            static size_t getColumnNumber() noexcept { return 14; }

            static const std::string &getColumnName(size_t index) noexcept(false);

            Json::Value toJson() const;

            Json::Value toMasqueradedJson(const std::vector<std::string> &pMasqueradingVector) const;
            /// Relationship interfaces
        private:
            friend drogon::orm::Mapper<Removed>;
#ifdef __cpp_impl_coroutine
            friend drogon::orm::CoroMapper<Removed>;
#endif

            static const std::vector<std::string> &insertColumns() noexcept;

            void outputArgs(drogon::orm::internal::SqlBinder &binder) const;

            const std::vector<std::string> updateColumns() const;

            void updateArgs(drogon::orm::internal::SqlBinder &binder) const;

            ///For mysql or sqlite3
            void updateId(const uint64_t id);

            std::shared_ptr<int64_t> id_;
            std::shared_ptr<std::string> username_;
            std::shared_ptr<std::string> motto_;
            std::shared_ptr<int32_t> region_;
            std::shared_ptr<std::string> avatar_;
            std::shared_ptr<std::string> avatarHash_;
            std::shared_ptr<int32_t> avatarFrame_;
            std::shared_ptr<std::string> clan_;
            std::shared_ptr<std::string> permission_;
            std::shared_ptr<std::string> password_;
            std::shared_ptr<std::string> email_;
            std::shared_ptr<std::string> data_;
            std::shared_ptr<::trantor::Date> timestamp_;
            std::shared_ptr<bool> recoverable_;
            struct MetaData {
                const std::string colName_;
                const std::string colType_;
                const std::string colDatabaseType_;
                const ssize_t colLength_;
                const bool isAutoVal_;
                const bool isPrimaryKey_;
                const bool notNull_;
            };
            static const std::vector<MetaData> metaData_;
            bool dirtyFlag_[14] = {false};
        public:
            static const std::string &sqlForFindingByPrimaryKey() {
                static const std::string sql = "select * from " + tableName + " where id = $1";
                return sql;
            }

            static const std::string &sqlForDeletingByPrimaryKey() {
                static const std::string sql = "delete from " + tableName + " where id = $1";
                return sql;
            }

            std::string sqlForInserting(bool &needSelection) const {
                std::string sql = "insert into " + tableName + " (";
                size_t parametersCount = 0;
                needSelection = false;
                if (dirtyFlag_[0]) {
                    sql += "id,";
                    ++parametersCount;
                }
                if (dirtyFlag_[1]) {
                    sql += "username,";
                    ++parametersCount;
                }
                if (dirtyFlag_[2]) {
                    sql += "motto,";
                    ++parametersCount;
                }
                if (dirtyFlag_[3]) {
                    sql += "region,";
                    ++parametersCount;
                }
                if (dirtyFlag_[4]) {
                    sql += "avatar,";
                    ++parametersCount;
                }
                if (dirtyFlag_[5]) {
                    sql += "avatar_hash,";
                    ++parametersCount;
                }
                if (dirtyFlag_[6]) {
                    sql += "avatar_frame,";
                    ++parametersCount;
                }
                if (dirtyFlag_[7]) {
                    sql += "clan,";
                    ++parametersCount;
                }
                if (dirtyFlag_[8]) {
                    sql += "permission,";
                    ++parametersCount;
                }
                if (dirtyFlag_[9]) {
                    sql += "password,";
                    ++parametersCount;
                }
                if (dirtyFlag_[10]) {
                    sql += "email,";
                    ++parametersCount;
                }
                sql += "data,";
                ++parametersCount;
                if (!dirtyFlag_[11]) {
                    needSelection = true;
                }
                sql += "timestamp,";
                ++parametersCount;
                if (!dirtyFlag_[12]) {
                    needSelection = true;
                }
                sql += "recoverable,";
                ++parametersCount;
                if (!dirtyFlag_[13]) {
                    needSelection = true;
                }
                if (parametersCount > 0) {
                    sql[sql.length() - 1] = ')';
                    sql += " values (";
                } else
                    sql += ") values (";

                int placeholder = 1;
                char placeholderStr[64];
                size_t n = 0;
                if (dirtyFlag_[0]) {
                    n = sprintf(placeholderStr, "$%d,", placeholder++);
                    sql.append(placeholderStr, n);
                }
                if (dirtyFlag_[1]) {
                    n = sprintf(placeholderStr, "$%d,", placeholder++);
                    sql.append(placeholderStr, n);
                }
                if (dirtyFlag_[2]) {
                    n = sprintf(placeholderStr, "$%d,", placeholder++);
                    sql.append(placeholderStr, n);
                }
                if (dirtyFlag_[3]) {
                    n = sprintf(placeholderStr, "$%d,", placeholder++);
                    sql.append(placeholderStr, n);
                }
                if (dirtyFlag_[4]) {
                    n = sprintf(placeholderStr, "$%d,", placeholder++);
                    sql.append(placeholderStr, n);
                }
                if (dirtyFlag_[5]) {
                    n = sprintf(placeholderStr, "$%d,", placeholder++);
                    sql.append(placeholderStr, n);
                }
                if (dirtyFlag_[6]) {
                    n = sprintf(placeholderStr, "$%d,", placeholder++);
                    sql.append(placeholderStr, n);
                }
                if (dirtyFlag_[7]) {
                    n = sprintf(placeholderStr, "$%d,", placeholder++);
                    sql.append(placeholderStr, n);
                }
                if (dirtyFlag_[8]) {
                    n = sprintf(placeholderStr, "$%d,", placeholder++);
                    sql.append(placeholderStr, n);
                }
                if (dirtyFlag_[9]) {
                    n = sprintf(placeholderStr, "$%d,", placeholder++);
                    sql.append(placeholderStr, n);
                }
                if (dirtyFlag_[10]) {
                    n = sprintf(placeholderStr, "$%d,", placeholder++);
                    sql.append(placeholderStr, n);
                }
                if (dirtyFlag_[11]) {
                    n = sprintf(placeholderStr, "$%d,", placeholder++);
                    sql.append(placeholderStr, n);
                } else {
                    sql += "default,";
                }
                if (dirtyFlag_[12]) {
                    n = sprintf(placeholderStr, "$%d,", placeholder++);
                    sql.append(placeholderStr, n);
                } else {
                    sql += "default,";
                }
                if (dirtyFlag_[13]) {
                    n = sprintf(placeholderStr, "$%d,", placeholder++);
                    sql.append(placeholderStr, n);
                } else {
                    sql += "default,";
                }
                if (parametersCount > 0) {
                    sql.resize(sql.length() - 1);
                }
                if (needSelection) {
                    sql.append(") returning *");
                } else {
                    sql.append(1, ')');
                }
                LOG_TRACE << sql;
                return sql;
            }
        };
    } // namespace techluster
} // namespace drogon_model
