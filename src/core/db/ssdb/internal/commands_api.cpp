/*  Copyright (C) 2014-2016 FastoGT. All right reserved.

    This file is part of FastoNoSQL.

    FastoNoSQL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FastoNoSQL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FastoNoSQL.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "core/db/ssdb/internal/commands_api.h"

#include <common/convert2string.h>

#include "core/db_key.h"

#include "core/db/ssdb/db_connection.h"

#include "global/global.h"

namespace fastonosql {
namespace core {
namespace ssdb {

common::Error info(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  ServerInfo::Stats statsout;
  common::Error err = ssdb->Info(argc == 1 ? argv[0] : 0, &statsout);
  if (err && err->isError()) {
    return err;
  }

  ServerInfo sinf(statsout);
  common::StringValue* val = common::Value::createStringValue(sinf.ToString());
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error dbsize(internal::CommandHandler* handler,
                     int argc,
                     const char** argv,
                     FastoObject* out) {
  UNUSED(argc);
  UNUSED(argv);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t dbsize = 0;
  common::Error err = ssdb->DBsize(&dbsize);
  if (err && err->isError()) {
    return err;
  }

  common::FundamentalValue* val = common::Value::createUIntegerValue(dbsize);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error auth(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  common::Error err = ssdb->Auth(argv[0]);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error get(internal::CommandHandler* handler,
                  int argc,
                  const char** argv,
                  FastoObject* out) {
  UNUSED(argc);

  NKey key(argv[0]);
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  NDbKValue key_loaded;
  common::Error err = ssdb->Get(key, &key_loaded);
  if (err && err->isError()) {
    return err;
  }

  NValue val = key_loaded.Value();
  common::Value* copy = val->deepCopy();
  FastoObject* child = new FastoObject(out, copy, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error select(internal::CommandHandler* handler,
                     int argc,
                     const char** argv,
                     FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  common::Error err = ssdb->Select(argv[0], nullptr);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error set(internal::CommandHandler* handler,
                  int argc,
                  const char** argv,
                  FastoObject* out) {
  UNUSED(argc);

  NKey key(argv[0]);
  NValue string_val(common::Value::createStringValue(argv[1]));
  NDbKValue kv(key, string_val);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  NDbKValue key_added;
  common::Error err = ssdb->Set(kv, &key_added);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error setx(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  common::Error err = ssdb->Setx(argv[0], argv[1], common::ConvertFromString<int>(argv[2]));
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error del(internal::CommandHandler* handler,
                  int argc,
                  const char** argv,
                  FastoObject* out) {
  NKeys keysdel;
  for (int i = 0; i < argc; ++i) {
    keysdel.push_back(NKey(argv[i]));
  }

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  NKeys keys_deleted;
  common::Error err = ssdb->Delete(keysdel, &keys_deleted);
  if (err && err->isError()) {
    return err;
  }

  common::FundamentalValue* val = common::Value::createUIntegerValue(keys_deleted.size());
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error rename(internal::CommandHandler* handler,
                     int argc,
                     const char** argv,
                     FastoObject* out) {
  UNUSED(argc);

  NKey key(argv[0]);
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  common::Error err = ssdb->Rename(key, argv[1]);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error set_ttl(internal::CommandHandler* handler,
                      int argc,
                      const char** argv,
                      FastoObject* out) {
  UNUSED(out);
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  NKey key(argv[0]);
  ttl_t ttl = common::ConvertFromString<ttl_t>(argv[1]);
  common::Error err = ssdb->SetTTL(key, ttl);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error incr(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t ret = 0;
  common::Error err = ssdb->Incr(argv[0], common::ConvertFromString<int64_t>(argv[1]), &ret);
  if (err && err->isError()) {
    return err;
  }

  common::FundamentalValue* val = common::Value::createIntegerValue(ret);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error keys(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysout;
  common::Error err =
      ssdb->Keys(argv[0], argv[1], common::ConvertFromString<uint64_t>(argv[2]), &keysout);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < keysout.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(keysout[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error scan(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysout;
  common::Error err =
      ssdb->Scan(argv[0], argv[1], common::ConvertFromString<uint64_t>(argv[2]), &keysout);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < keysout.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(keysout[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error rscan(internal::CommandHandler* handler,
                    int argc,
                    const char** argv,
                    FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysout;
  common::Error err =
      ssdb->Rscan(argv[0], argv[1], common::ConvertFromString<uint64_t>(argv[2]), &keysout);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < keysout.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(keysout[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error multi_get(internal::CommandHandler* handler,
                        int argc,
                        const char** argv,
                        FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysget;
  for (int i = 0; i < argc; ++i) {
    keysget.push_back(argv[i]);
  }

  std::vector<std::string> keysout;
  common::Error err = ssdb->MultiGet(keysget, &keysout);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < keysout.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(keysout[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error multi_set(internal::CommandHandler* handler,
                        int argc,
                        const char** argv,
                        FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::map<std::string, std::string> keysset;
  for (int i = 0; i < argc; i += 2) {
    keysset[argv[i]] = argv[i + 1];
  }

  common::Error err = ssdb->MultiSet(keysset);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error multi_del(internal::CommandHandler* handler,
                        int argc,
                        const char** argv,
                        FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysget;
  for (int i = 0; i < argc; ++i) {
    keysget.push_back(argv[i]);
  }

  common::Error err = ssdb->MultiDel(keysget);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error hget(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::string ret;
  common::Error err = ssdb->Hget(argv[0], argv[1], &ret);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue(ret);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error hgetall(internal::CommandHandler* handler,
                      int argc,
                      const char** argv,
                      FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysout;
  common::Error err = ssdb->Hgetall(argv[0], &keysout);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < keysout.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(keysout[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error hset(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  common::Error err = ssdb->Hset(argv[0], argv[1], argv[2]);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error hdel(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  common::Error err = ssdb->Hdel(argv[0], argv[1]);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error hincr(internal::CommandHandler* handler,
                    int argc,
                    const char** argv,
                    FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  common::Error err =
      ssdb->Hincr(argv[0], argv[1], common::ConvertFromString<int64_t>(argv[2]), &res);
  if (err && err->isError()) {
    return err;
  }

  common::FundamentalValue* val = common::Value::createIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error hsize(internal::CommandHandler* handler,
                    int argc,
                    const char** argv,
                    FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  common::Error err = ssdb->Hsize(argv[0], &res);
  if (err && err->isError()) {
    return err;
  }

  common::FundamentalValue* val = common::Value::createIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error hclear(internal::CommandHandler* handler,
                     int argc,
                     const char** argv,
                     FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  common::Error err = ssdb->Hclear(argv[0], &res);
  if (err && err->isError()) {
    return err;
  }

  common::FundamentalValue* val = common::Value::createIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error hkeys(internal::CommandHandler* handler,
                    int argc,
                    const char** argv,
                    FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysout;
  common::Error err = ssdb->Hkeys(argv[0], argv[1], argv[2],
                                  common::ConvertFromString<uint64_t>(argv[3]), &keysout);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < keysout.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(keysout[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error hscan(internal::CommandHandler* handler,
                    int argc,
                    const char** argv,
                    FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysout;
  common::Error err = ssdb->Hscan(argv[0], argv[1], argv[2],
                                  common::ConvertFromString<uint64_t>(argv[3]), &keysout);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < keysout.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(keysout[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error hrscan(internal::CommandHandler* handler,
                     int argc,
                     const char** argv,
                     FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysout;
  common::Error err = ssdb->Hrscan(argv[0], argv[1], argv[2],
                                   common::ConvertFromString<uint64_t>(argv[3]), &keysout);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < keysout.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(keysout[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error multi_hget(internal::CommandHandler* handler,
                         int argc,
                         const char** argv,
                         FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysget;
  for (int i = 1; i < argc; ++i) {
    keysget.push_back(argv[i]);
  }

  std::vector<std::string> keysout;
  common::Error err = ssdb->MultiHget(argv[0], keysget, &keysout);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < keysout.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(keysout[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error multi_hset(internal::CommandHandler* handler,
                         int argc,
                         const char** argv,
                         FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::map<std::string, std::string> keys;
  for (int i = 1; i < argc; i += 2) {
    keys[argv[i]] = argv[i + 1];
  }

  common::Error err = ssdb->MultiHset(argv[0], keys);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error zget(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t ret;
  common::Error err = ssdb->Zget(argv[0], argv[1], &ret);
  if (err && err->isError()) {
    return err;
  }

  common::FundamentalValue* val = common::Value::createIntegerValue(ret);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error zset(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  common::Error err = ssdb->Zset(argv[0], argv[1], common::ConvertFromString<int64_t>(argv[2]));
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error zdel(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  common::Error err = ssdb->Zdel(argv[0], argv[1]);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error zincr(internal::CommandHandler* handler,
                    int argc,
                    const char** argv,
                    FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t ret = 0;
  common::Error err =
      ssdb->Zincr(argv[0], argv[1], common::ConvertFromString<int64_t>(argv[2]), &ret);
  if (err && err->isError()) {
    return err;
  }

  common::FundamentalValue* val = common::Value::createIntegerValue(ret);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error zsize(internal::CommandHandler* handler,
                    int argc,
                    const char** argv,
                    FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  common::Error err = ssdb->Zsize(argv[0], &res);
  if (err && err->isError()) {
    return err;
  }

  common::FundamentalValue* val = common::Value::createIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error zclear(internal::CommandHandler* handler,
                     int argc,
                     const char** argv,
                     FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  common::Error err = ssdb->Zclear(argv[0], &res);
  if (err && err->isError()) {
    return err;
  }

  common::FundamentalValue* val = common::Value::createIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error zrank(internal::CommandHandler* handler,
                    int argc,
                    const char** argv,
                    FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  common::Error err = ssdb->Zrank(argv[0], argv[1], &res);
  if (err && err->isError()) {
    return err;
  }

  common::FundamentalValue* val = common::Value::createIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error zrrank(internal::CommandHandler* handler,
                     int argc,
                     const char** argv,
                     FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  common::Error err = ssdb->Zrrank(argv[0], argv[1], &res);
  if (err && err->isError()) {
    return err;
  }

  common::FundamentalValue* val = common::Value::createIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error zrange(internal::CommandHandler* handler,
                     int argc,
                     const char** argv,
                     FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> res;
  common::Error err = ssdb->Zrange(argv[0], common::ConvertFromString<uint64_t>(argv[1]),
                                   common::ConvertFromString<uint64_t>(argv[2]), &res);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < res.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(res[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error zrrange(internal::CommandHandler* handler,
                      int argc,
                      const char** argv,
                      FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> res;
  common::Error err = ssdb->Zrrange(argv[0], common::ConvertFromString<uint64_t>(argv[1]),
                                    common::ConvertFromString<uint64_t>(argv[2]), &res);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < res.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(res[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error zkeys(internal::CommandHandler* handler,
                    int argc,
                    const char** argv,
                    FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> res;
  int64_t st = common::ConvertFromString<int64_t>(argv[2]);
  int64_t end = common::ConvertFromString<int64_t>(argv[3]);
  common::Error err =
      ssdb->Zkeys(argv[0], argv[1], &st, &end, common::ConvertFromString<uint64_t>(argv[5]), &res);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < res.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(res[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error zscan(internal::CommandHandler* handler,
                    int argc,
                    const char** argv,
                    FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> res;
  int64_t st = common::ConvertFromString<int64_t>(argv[2]);
  int64_t end = common::ConvertFromString<int64_t>(argv[3]);
  common::Error err =
      ssdb->Zscan(argv[0], argv[1], &st, &end, common::ConvertFromString<uint64_t>(argv[4]), &res);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < res.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(res[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error zrscan(internal::CommandHandler* handler,
                     int argc,
                     const char** argv,
                     FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> res;
  int64_t st = common::ConvertFromString<int64_t>(argv[2]);
  int64_t end = common::ConvertFromString<int64_t>(argv[3]);
  common::Error err =
      ssdb->Zrscan(argv[0], argv[1], &st, &end, common::ConvertFromString<uint64_t>(argv[4]), &res);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < res.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(res[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error multi_zget(internal::CommandHandler* handler,
                         int argc,
                         const char** argv,
                         FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysget;
  for (int i = 1; i < argc; ++i) {
    keysget.push_back(argv[i]);
  }

  std::vector<std::string> res;
  common::Error err = ssdb->MultiZget(argv[0], keysget, &res);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < res.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(res[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error multi_zset(internal::CommandHandler* handler,
                         int argc,
                         const char** argv,
                         FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::map<std::string, int64_t> keysget;
  for (int i = 1; i < argc; i += 2) {
    keysget[argv[i]] = common::ConvertFromString<int64_t>(argv[i + 1]);
  }

  common::Error err = ssdb->MultiZset(argv[0], keysget);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error multi_zdel(internal::CommandHandler* handler,
                         int argc,
                         const char** argv,
                         FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::vector<std::string> keysget;
  for (int i = 1; i < argc; ++i) {
    keysget.push_back(argv[i]);
  }

  common::Error err = ssdb->MultiZdel(argv[0], keysget);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error qpush(internal::CommandHandler* handler,
                    int argc,
                    const char** argv,
                    FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  common::Error err = ssdb->Qpush(argv[0], argv[1]);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error qpop(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::string ret;
  common::Error err = ssdb->Qpop(argv[0], &ret);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue(ret);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error qslice(internal::CommandHandler* handler,
                     int argc,
                     const char** argv,
                     FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t begin = common::ConvertFromString<int64_t>(argv[1]);
  int64_t end = common::ConvertFromString<int64_t>(argv[2]);

  std::vector<std::string> keysout;
  common::Error err = ssdb->Qslice(argv[0], begin, end, &keysout);
  if (err && err->isError()) {
    return err;
  }

  common::ArrayValue* ar = common::Value::createArrayValue();
  for (size_t i = 0; i < keysout.size(); ++i) {
    common::StringValue* val = common::Value::createStringValue(keysout[i]);
    ar->append(val);
  }
  FastoObjectArray* child = new FastoObjectArray(out, ar, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error qclear(internal::CommandHandler* handler,
                     int argc,
                     const char** argv,
                     FastoObject* out) {
  UNUSED(argc);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  int64_t res = 0;
  common::Error err = ssdb->Qclear(argv[0], &res);
  if (err && err->isError()) {
    return err;
  }

  common::FundamentalValue* val = common::Value::createIntegerValue(res);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error dbkcount(internal::CommandHandler* handler,
                       int argc,
                       const char** argv,
                       FastoObject* out) {
  UNUSED(argc);
  UNUSED(argv);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  size_t dbkcount = 0;
  common::Error err = ssdb->DBkcount(&dbkcount);
  if (err && err->isError()) {
    return err;
  }

  common::FundamentalValue* val = common::Value::createUIntegerValue(dbkcount);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error help(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  std::string answer;
  common::Error err = ssdb->Help(argc, argv, &answer);
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue(answer);
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error flushdb(internal::CommandHandler* handler,
                      int argc,
                      const char** argv,
                      FastoObject* out) {
  UNUSED(argc);
  UNUSED(argv);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  common::Error err = ssdb->FlushDB();
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

common::Error quit(internal::CommandHandler* handler,
                   int argc,
                   const char** argv,
                   FastoObject* out) {
  UNUSED(argc);
  UNUSED(argv);

  DBConnection* ssdb = static_cast<DBConnection*>(handler);
  common::Error err = ssdb->Quit();
  if (err && err->isError()) {
    return err;
  }

  common::StringValue* val = common::Value::createStringValue("OK");
  FastoObject* child = new FastoObject(out, val, ssdb->Delimiter());
  out->AddChildren(child);
  return common::Error();
}

}  // namespace ssdb
}  // namespace core
}  // namespace fastonosql
