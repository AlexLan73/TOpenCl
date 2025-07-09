// my_msgpack_adapters.h
#pragma once

#include <msgpack.hpp>
#include "interfaces/i_memory_config_channel.h"

// Для enum
MSGPACK_ADD_ENUM(logger_send_enum_memory);

namespace msgpack {
  MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
    namespace adaptor {

      // SValueDt
      template<>
      struct pack<SValueDt> {
        template <typename Stream>
        packer<Stream>& operator()(msgpack::packer<Stream>& o, SValueDt const& v) const {
          o.pack_array(2);
          o.pack(v.value);
          o.pack(v.ticks);
          return o;
        }
      };
      template<>
      struct convert<SValueDt> {
        msgpack::object const& operator()(msgpack::object const& o, SValueDt& v) const {
          if (o.type != msgpack::type::ARRAY || o.via.array.size != 2) throw msgpack::type_error();
          v.value = o.via.array.ptr[0].as<double>();
          v.ticks = o.via.array.ptr[1].as<int64_t>();
          return o;
        }
      };

      // ILoggerChannel
      template<>
      struct pack<ILoggerChannel> {
        template <typename Stream>
        packer<Stream>& operator()(msgpack::packer<Stream>& o, ILoggerChannel const& v) const {
          o.pack_array(5);
          o.pack(v.id);
          o.pack(v.module);
          o.pack(v.log);
          o.pack(v.code);
          o.pack(v.ticks);
          return o;
        }
      };
      template<>
      struct convert<ILoggerChannel> {
        msgpack::object const& operator()(msgpack::object const& o, ILoggerChannel& v) const {
          if (o.type != msgpack::type::ARRAY || o.via.array.size != 5) throw msgpack::type_error();
          v.id = o.via.array.ptr[0].as<uint32_t>();
          v.module = o.via.array.ptr[1].as<std::string>();
          v.log = o.via.array.ptr[2].as<std::string>();
          v.code = o.via.array.ptr[3].as<logger_send_enum_memory>();
          v.ticks = o.via.array.ptr[4].as<int64_t>();
          return o;
        }
      };

      // IIdValueDtChannel
      template<>
      struct pack<IIdValueDtChannel> {
        template <typename Stream>
        packer<Stream>& operator()(msgpack::packer<Stream>& o, IIdValueDtChannel const& v) const {
          o.pack_array(2);
          o.pack(v.id);
          o.pack(v.value_dt);
          return o;
        }
      };
      template<>
      struct convert<IIdValueDtChannel> {
        msgpack::object const& operator()(msgpack::object const& o, IIdValueDtChannel& v) const {
          if (o.type != msgpack::type::ARRAY || o.via.array.size != 2) throw msgpack::type_error();
          v.id = o.via.array.ptr[0].as<uint32_t>();
          v.value_dt = o.via.array.ptr[1].as<SValueDt>();
          return o;
        }
      };

      // IIdVecValueDtChannel
      template<>
      struct pack<IIdVecValueDtChannel> {
        template <typename Stream>
        packer<Stream>& operator()(msgpack::packer<Stream>& o, IIdVecValueDtChannel const& v) const {
          o.pack_array(2);
          o.pack(v.id);
          o.pack(v.vec_value_dt);
          return o;
        }
      };
      template<>
      struct convert<IIdVecValueDtChannel> {
        msgpack::object const& operator()(msgpack::object const& o, IIdVecValueDtChannel& v) const {
          if (o.type != msgpack::type::ARRAY || o.via.array.size != 2) throw msgpack::type_error();
          v.id = o.via.array.ptr[0].as<uint32_t>();
          v.vec_value_dt = o.via.array.ptr[1].as<std::vector<SValueDt>>();
          return o;
        }
      };

    } // namespace adaptor
  } // MSGPACK_API_VERSION_NAMESPACE
} // namespace msgpack
