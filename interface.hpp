/* Исходный код с реализацей функций-оберток над стандартными функциями ZMQ для отправки и получения данных.
      Разработчик: Ляшун Д. С. */
#pragma once
#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <optional>

namespace nZMQInterface {
    enum eDataType { // Представляет тип получаемых и отправляемых сообщенией
        COORDINATE = 0,
        COORDINATE_END,
        ALTITUDE,
        ALTITUDE_END
    };

    struct rData { // Представляет форму, по которым производители передают координаты и высоты
        int32_t m_l_x_coord;
        int32_t m_l_y_coord;
        int32_t m_l_altitude;
        int32_t m_e_type;
        rData() = default;
        rData(const eDataType& p_e_type, const int32_t& p_l_x_coord, const int32_t& p_l_y_coord) {
            m_l_x_coord = p_l_x_coord;
            m_l_y_coord = p_l_y_coord;
            m_l_altitude = -1;
            m_e_type = p_e_type;
        }
        rData(const eDataType& p_e_type, const int32_t& p_l_altitude) {
            m_l_x_coord = -1;
            m_l_y_coord = -1;
            m_l_altitude = p_l_altitude;
            m_e_type = p_e_type;
        }
        rData(const eDataType& p_e_type) {
            m_l_x_coord = -1;
            m_l_y_coord = -1;
            m_l_altitude = -1;
            m_e_type = p_e_type;
        }
        rData(const std::string& p_s_buffer) {
            short i = 0;
            std::function<int32_t(const std::string&, short&)> get_number { // Лямбда функция, которая извлекает одно число из строкового буфера
                [](const std::string& p_s_buffer, short& i) {
                    std::string s_res;
                    while (i < p_s_buffer.size() && p_s_buffer[i] != ' ') {
                        s_res.push_back(p_s_buffer[i]);
                        ++i;
                    }
                    ++i;
                    return std::stoi(s_res);
                }
            };
            m_e_type = get_number(p_s_buffer, i);
            m_l_x_coord = get_number(p_s_buffer, i);
            m_l_y_coord = get_number(p_s_buffer, i);
            m_l_altitude = get_number(p_s_buffer, i);
        }
    };

    std::optional<zmq::message_t> recieve_message(zmq::socket_t& p_socket) { // Функция получения сообщения по сокету p_socket
        zmq::message_t msg;
        if (auto res = p_socket.recv(msg, zmq::recv_flags::none)) { // В res хранится число успешно полученных байт сообщения или ничего в случае ошибки
            return msg;
        }
        return std::nullopt;
    }

    int32_t send_message(zmq::socket_t& p_socket, zmq::message_t& p_msg) { // Функция отправки сообщения p_msg по сокету p_socket
        int32_t l_msg_size = p_msg.size();
        auto res = p_socket.send(p_msg, zmq::send_flags::none);
        if (res.value() != l_msg_size) { // Если число успешно отправленных байт сообщения не совпадает с его размером, то вернуть -1 как факт ошибки
            return -1;
        }
        return 0;
    }

    int32_t send_data(zmq::socket_t& p_socket, const std::string& p_data) { // Функция отправки данных p_data по сокету p_socket
        zmq::message_t msg(p_data);
        return send_message(p_socket, msg);
    }

    int32_t send_data(zmq::socket_t& p_socket, const rData& p_data) { // Функция отправки p_data типа rData по сокету p_socket
        return send_data(p_socket, std::to_string(p_data.m_e_type) + " " + std::to_string(p_data.m_l_x_coord) + " " + std::to_string(p_data.m_l_y_coord) + " " + std::to_string(p_data.m_l_altitude));
    }
    
    std::optional<rData> recieve_data(zmq::socket_t& p_socket) { // Функция получения объекта типа rData по сокету p_socket
        zmq::message_t msg;
        auto res = p_socket.recv(msg, zmq::recv_flags::none);
        if (res.has_value()) {
            return rData(msg.to_string());
        }
        return std::nullopt;
    }
}

