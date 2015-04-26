//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2014-2015 KONDO Takatoshi
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
#ifndef MSGPACK_TYPE_FIX_ARRAY_HPP
#define MSGPACK_TYPE_FIX_ARRAY_HPP

#include "msgpack/versioning.hpp"
#include "msgpack/adaptor/adaptor_base.hpp"
#include "msgpack/adaptor/check_container_size.hpp"

namespace msgpack {

/// @cond
MSGPACK_API_VERSION_NAMESPACE(v1) {
/// @endcond

namespace adaptor {

template<typename T, size_t N>
struct convert <T[N]> {
    msgpack::object const& operator()(msgpack::object const& o, T(&v)[N]) const {
        if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
        if (o.via.array.size != N) throw msgpack::type_error();

        for (size_t i = 0; i < N; i++)
            o.via.array.ptr[i].convert(v[i]);

        return o;
    }
};

template<size_t N>
struct convert<char[N]>{
    msgpack::object const& operator()(msgpack::object const& o, char(&v)[N]) const {
        if (o.type != msgpack::type::STR) throw msgpack::type_error();
        if (o.via.str.size + 1 > N) throw msgpack::type_error();

        std::memcpy(v, o.via.str.ptr, o.via.str.size);
        v[o.via.str.size] = '\0';

        return o;
    }
};

template<typename T, size_t N>
struct pack<T[N]> {
    template <typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, const T v[N]) const {
        o.pack_array(N);
        for (size_t i = 0; i < N; i++)
            o.pack(v[i]);
        return o;
    }
};

template<typename T, size_t N>
struct object_with_zone<T[N]> {
    void operator()(msgpack::object::with_zone& o, const T v[N]) const {
        o.type = type::ARRAY;
        o.via.array.size = N;
        o.via.array.ptr = static_cast<msgpack::object*>(
            o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));

        for (size_t i = 0; i < N; i++)
            o.via.array.ptr[i] = msgpack::object(v[i], o.zone);
    }
};


template<typename T, size_t N1, size_t N2>
struct convert<T[N1][N2]>{
    msgpack::object const& operator()(msgpack::object const& o, T(&v)[N1][N2]) const {
        if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
        if (o.via.array.size != N1) throw msgpack::type_error();

        for (size_t i = 0; i < N1; i++)
        {
            msgpack::object const& oc = o.via.array.ptr[i];

            if (oc.type != msgpack::type::ARRAY) throw msgpack::type_error();
            if (oc.via.array.size != N2) throw msgpack::type_error();

            for (size_t j = 0; j < N2; j++)
                oc.via.array.ptr[j].convert(v[i][j]);
        }

        return o;
    }
};

template<typename T, size_t N1, size_t N2>
struct pack<T[N1][N2]> {
    template <typename Stream>
    packer<Stream>& operator()(msgpack::packer<Stream>& o, const T v[N1][N2]) const {
        o.pack_array(N1);
        for (size_t i = 0; i < N1; i++)
        {
            o.pack_array(N2);
            for (size_t j = 0; j < N2; j++)
                o.pack(v[i][j]);
        }
        return o;
    }
};

template<typename T, size_t N1, size_t N2>
struct object_with_zone<T[N1][N2]> {
    void operator()(msgpack::object::with_zone& o, const T v[N1][N2]) const {
        o.type = type::ARRAY;
        o.via.array.size = N1;
        o.via.array.ptr = static_cast<msgpack::object*>(
            o.zone.allocate_align(sizeof(msgpack::object) * o.via.array.size));

        for (size_t i = 0; i < N1; i++)
        {
            msgpack::object& oc = o.via.array.ptr[i];

            oc.type = type::ARRAY;
            oc.via.array.size = N2;
            oc.via.array.ptr = static_cast<msgpack::object*>(
                o.zone.allocate_align(sizeof(msgpack::object) * oc.via.array.size));

            for (size_t j = 0; j < N2; j++)
                oc.via.array.ptr[j] = msgpack::object(v[i][j], o.zone);
        }
    }
};

} // namespace adaptor

/// @cond
} // MSGPACK_API_VERSION_NAMESPACE(v1)
/// @endcond

} // namespace msgpack

#endif // MSGPACK_TYPE_FIX_ARRAY_HPP
