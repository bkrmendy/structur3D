//
// Created by Berci on 2020. 12. 02..
//

#ifndef STRUCTUR3D_BASE_SERIALIZATOR_H
#define STRUCTUR3D_BASE_SERIALIZATOR_H

#include <bitsery/bitsery.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/traits/vector.h>

namespace S3D {
    class Serializator {
        using Buffer = std::vector<uint8_t>;
        using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
        using InputAdapter = bitsery::InputBufferAdapter<Buffer>;
    public:

        Buffer make_buffer() {
            return Buffer{};
        }

        template<typename T>
        size_t serialize(Buffer& to, T& thing) {
            auto size = bitsery::quickSerialization<OutputAdapter>(to, thing);
            return size;
        }


        template<typename T>
        bool deserialize(Buffer& from, T& to, unsigned int size) {
            auto state = bitsery::quickDeserialization<InputAdapter>({ from.begin(), size }, to);
            return state.first == bitsery::ReaderError::NoError && state.second;
        }
    };
}

#endif //STRUCTUR3D_BASE_SERIALIZATOR_H
