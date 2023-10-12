// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*!
 * @file AMM_Extended.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifndef _FAST_DDS_GENERATED_AMM_AMM_EXTENDED_H_
#define _FAST_DDS_GENERATED_AMM_AMM_EXTENDED_H_


#include <fastrtps/utils/fixed_size_string.hpp>

#include <stdint.h>
#include <array>
#include <string>
#include <vector>
#include <map>
#include <bitset>

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#define eProsima_user_DllExport __declspec( dllexport )
#else
#define eProsima_user_DllExport
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define eProsima_user_DllExport
#endif  // _WIN32

#if defined(_WIN32)
#if defined(EPROSIMA_USER_DLL_EXPORT)
#if defined(AMM_Extended_SOURCE)
#define AMM_Extended_DllAPI __declspec( dllexport )
#else
#define AMM_Extended_DllAPI __declspec( dllimport )
#endif // AMM_Extended_SOURCE
#else
#define AMM_Extended_DllAPI
#endif  // EPROSIMA_USER_DLL_EXPORT
#else
#define AMM_Extended_DllAPI
#endif // _WIN32

namespace eprosima {
namespace fastcdr {
class Cdr;
} // namespace fastcdr
} // namespace eprosima


namespace AMM {
    /*!
     * @brief This class represents the structure Tick defined by the user in the IDL file.
     * @ingroup AMM_EXTENDED
     */
    class Tick
    {
    public:

        /*!
         * @brief Default constructor.
         */
        eProsima_user_DllExport Tick();

        /*!
         * @brief Default destructor.
         */
        eProsima_user_DllExport ~Tick();

        /*!
         * @brief Copy constructor.
         * @param x Reference to the object AMM::Tick that will be copied.
         */
        eProsima_user_DllExport Tick(
                const Tick& x);

        /*!
         * @brief Move constructor.
         * @param x Reference to the object AMM::Tick that will be copied.
         */
        eProsima_user_DllExport Tick(
                Tick&& x) noexcept;

        /*!
         * @brief Copy assignment.
         * @param x Reference to the object AMM::Tick that will be copied.
         */
        eProsima_user_DllExport Tick& operator =(
                const Tick& x);

        /*!
         * @brief Move assignment.
         * @param x Reference to the object AMM::Tick that will be copied.
         */
        eProsima_user_DllExport Tick& operator =(
                Tick&& x) noexcept;

        /*!
         * @brief Comparison operator.
         * @param x AMM::Tick object to compare.
         */
        eProsima_user_DllExport bool operator ==(
                const Tick& x) const;

        /*!
         * @brief Comparison operator.
         * @param x AMM::Tick object to compare.
         */
        eProsima_user_DllExport bool operator !=(
                const Tick& x) const;

        /*!
         * @brief This function sets a value in member frame
         * @param _frame New value for member frame
         */
        eProsima_user_DllExport void frame(
                int64_t _frame);

        /*!
         * @brief This function returns the value of member frame
         * @return Value of member frame
         */
        eProsima_user_DllExport int64_t frame() const;

        /*!
         * @brief This function returns a reference to member frame
         * @return Reference to member frame
         */
        eProsima_user_DllExport int64_t& frame();

        /*!
         * @brief This function sets a value in member time
         * @param _time New value for member time
         */
        eProsima_user_DllExport void time(
                float _time);

        /*!
         * @brief This function returns the value of member time
         * @return Value of member time
         */
        eProsima_user_DllExport float time() const;

        /*!
         * @brief This function returns a reference to member time
         * @return Reference to member time
         */
        eProsima_user_DllExport float& time();


        /*!
        * @brief This function returns the maximum serialized size of an object
        * depending on the buffer alignment.
        * @param current_alignment Buffer alignment.
        * @return Maximum serialized size.
        */
        eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
                size_t current_alignment = 0);

        /*!
         * @brief This function returns the serialized size of a data depending on the buffer alignment.
         * @param data Data which is calculated its serialized size.
         * @param current_alignment Buffer alignment.
         * @return Serialized size.
         */
        eProsima_user_DllExport static size_t getCdrSerializedSize(
                const AMM::Tick& data,
                size_t current_alignment = 0);


        /*!
         * @brief This function serializes an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void serialize(
                eprosima::fastcdr::Cdr& cdr) const;

        /*!
         * @brief This function deserializes an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void deserialize(
                eprosima::fastcdr::Cdr& cdr);



        /*!
         * @brief This function returns the maximum serialized size of the Key of an object
         * depending on the buffer alignment.
         * @param current_alignment Buffer alignment.
         * @return Maximum serialized size.
         */
        eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
                size_t current_alignment = 0);

        /*!
         * @brief This function tells you if the Key has been defined for this type
         */
        eProsima_user_DllExport static bool isKeyDefined();

        /*!
         * @brief This function serializes the key members of an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void serializeKey(
                eprosima::fastcdr::Cdr& cdr) const;

    private:

        int64_t m_frame;
        float m_time;

    };
    /*!
     * @brief This class represents the structure InstrumentData defined by the user in the IDL file.
     * @ingroup AMM_EXTENDED
     */
    class InstrumentData
    {
    public:

        /*!
         * @brief Default constructor.
         */
        eProsima_user_DllExport InstrumentData();

        /*!
         * @brief Default destructor.
         */
        eProsima_user_DllExport ~InstrumentData();

        /*!
         * @brief Copy constructor.
         * @param x Reference to the object AMM::InstrumentData that will be copied.
         */
        eProsima_user_DllExport InstrumentData(
                const InstrumentData& x);

        /*!
         * @brief Move constructor.
         * @param x Reference to the object AMM::InstrumentData that will be copied.
         */
        eProsima_user_DllExport InstrumentData(
                InstrumentData&& x) noexcept;

        /*!
         * @brief Copy assignment.
         * @param x Reference to the object AMM::InstrumentData that will be copied.
         */
        eProsima_user_DllExport InstrumentData& operator =(
                const InstrumentData& x);

        /*!
         * @brief Move assignment.
         * @param x Reference to the object AMM::InstrumentData that will be copied.
         */
        eProsima_user_DllExport InstrumentData& operator =(
                InstrumentData&& x) noexcept;

        /*!
         * @brief Comparison operator.
         * @param x AMM::InstrumentData object to compare.
         */
        eProsima_user_DllExport bool operator ==(
                const InstrumentData& x) const;

        /*!
         * @brief Comparison operator.
         * @param x AMM::InstrumentData object to compare.
         */
        eProsima_user_DllExport bool operator !=(
                const InstrumentData& x) const;

        /*!
         * @brief This function copies the value in member instrument
         * @param _instrument New value to be copied in member instrument
         */
        eProsima_user_DllExport void instrument(
                const std::string& _instrument);

        /*!
         * @brief This function moves the value in member instrument
         * @param _instrument New value to be moved in member instrument
         */
        eProsima_user_DllExport void instrument(
                std::string&& _instrument);

        /*!
         * @brief This function returns a constant reference to member instrument
         * @return Constant reference to member instrument
         */
        eProsima_user_DllExport const std::string& instrument() const;

        /*!
         * @brief This function returns a reference to member instrument
         * @return Reference to member instrument
         */
        eProsima_user_DllExport std::string& instrument();
        /*!
         * @brief This function copies the value in member payload
         * @param _payload New value to be copied in member payload
         */
        eProsima_user_DllExport void payload(
                const eprosima::fastrtps::fixed_string<4096>& _payload);

        /*!
         * @brief This function moves the value in member payload
         * @param _payload New value to be moved in member payload
         */
        eProsima_user_DllExport void payload(
                eprosima::fastrtps::fixed_string<4096>&& _payload);

        /*!
         * @brief This function returns a constant reference to member payload
         * @return Constant reference to member payload
         */
        eProsima_user_DllExport const eprosima::fastrtps::fixed_string<4096>& payload() const;

        /*!
         * @brief This function returns a reference to member payload
         * @return Reference to member payload
         */
        eProsima_user_DllExport eprosima::fastrtps::fixed_string<4096>& payload();

        /*!
        * @brief This function returns the maximum serialized size of an object
        * depending on the buffer alignment.
        * @param current_alignment Buffer alignment.
        * @return Maximum serialized size.
        */
        eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
                size_t current_alignment = 0);

        /*!
         * @brief This function returns the serialized size of a data depending on the buffer alignment.
         * @param data Data which is calculated its serialized size.
         * @param current_alignment Buffer alignment.
         * @return Serialized size.
         */
        eProsima_user_DllExport static size_t getCdrSerializedSize(
                const AMM::InstrumentData& data,
                size_t current_alignment = 0);


        /*!
         * @brief This function serializes an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void serialize(
                eprosima::fastcdr::Cdr& cdr) const;

        /*!
         * @brief This function deserializes an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void deserialize(
                eprosima::fastcdr::Cdr& cdr);



        /*!
         * @brief This function returns the maximum serialized size of the Key of an object
         * depending on the buffer alignment.
         * @param current_alignment Buffer alignment.
         * @return Maximum serialized size.
         */
        eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
                size_t current_alignment = 0);

        /*!
         * @brief This function tells you if the Key has been defined for this type
         */
        eProsima_user_DllExport static bool isKeyDefined();

        /*!
         * @brief This function serializes the key members of an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void serializeKey(
                eprosima::fastcdr::Cdr& cdr) const;

    private:

        std::string m_instrument;
        eprosima::fastrtps::fixed_string<4096> m_payload;

    };
    /*!
     * @brief This class represents the structure Command defined by the user in the IDL file.
     * @ingroup AMM_EXTENDED
     */
    class Command
    {
    public:

        /*!
         * @brief Default constructor.
         */
        eProsima_user_DllExport Command();

        /*!
         * @brief Default destructor.
         */
        eProsima_user_DllExport ~Command();

        /*!
         * @brief Copy constructor.
         * @param x Reference to the object AMM::Command that will be copied.
         */
        eProsima_user_DllExport Command(
                const Command& x);

        /*!
         * @brief Move constructor.
         * @param x Reference to the object AMM::Command that will be copied.
         */
        eProsima_user_DllExport Command(
                Command&& x) noexcept;

        /*!
         * @brief Copy assignment.
         * @param x Reference to the object AMM::Command that will be copied.
         */
        eProsima_user_DllExport Command& operator =(
                const Command& x);

        /*!
         * @brief Move assignment.
         * @param x Reference to the object AMM::Command that will be copied.
         */
        eProsima_user_DllExport Command& operator =(
                Command&& x) noexcept;

        /*!
         * @brief Comparison operator.
         * @param x AMM::Command object to compare.
         */
        eProsima_user_DllExport bool operator ==(
                const Command& x) const;

        /*!
         * @brief Comparison operator.
         * @param x AMM::Command object to compare.
         */
        eProsima_user_DllExport bool operator !=(
                const Command& x) const;

        /*!
         * @brief This function copies the value in member message
         * @param _message New value to be copied in member message
         */
        eProsima_user_DllExport void message(
                const std::string& _message);

        /*!
         * @brief This function moves the value in member message
         * @param _message New value to be moved in member message
         */
        eProsima_user_DllExport void message(
                std::string&& _message);

        /*!
         * @brief This function returns a constant reference to member message
         * @return Constant reference to member message
         */
        eProsima_user_DllExport const std::string& message() const;

        /*!
         * @brief This function returns a reference to member message
         * @return Reference to member message
         */
        eProsima_user_DllExport std::string& message();

        /*!
        * @brief This function returns the maximum serialized size of an object
        * depending on the buffer alignment.
        * @param current_alignment Buffer alignment.
        * @return Maximum serialized size.
        */
        eProsima_user_DllExport static size_t getMaxCdrSerializedSize(
                size_t current_alignment = 0);

        /*!
         * @brief This function returns the serialized size of a data depending on the buffer alignment.
         * @param data Data which is calculated its serialized size.
         * @param current_alignment Buffer alignment.
         * @return Serialized size.
         */
        eProsima_user_DllExport static size_t getCdrSerializedSize(
                const AMM::Command& data,
                size_t current_alignment = 0);


        /*!
         * @brief This function serializes an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void serialize(
                eprosima::fastcdr::Cdr& cdr) const;

        /*!
         * @brief This function deserializes an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void deserialize(
                eprosima::fastcdr::Cdr& cdr);



        /*!
         * @brief This function returns the maximum serialized size of the Key of an object
         * depending on the buffer alignment.
         * @param current_alignment Buffer alignment.
         * @return Maximum serialized size.
         */
        eProsima_user_DllExport static size_t getKeyMaxCdrSerializedSize(
                size_t current_alignment = 0);

        /*!
         * @brief This function tells you if the Key has been defined for this type
         */
        eProsima_user_DllExport static bool isKeyDefined();

        /*!
         * @brief This function serializes the key members of an object using CDR serialization.
         * @param cdr CDR serialization object.
         */
        eProsima_user_DllExport void serializeKey(
                eprosima::fastcdr::Cdr& cdr) const;

    private:

        std::string m_message;

    };
} // namespace AMM

#endif // _FAST_DDS_GENERATED_AMM_AMM_EXTENDED_H_
