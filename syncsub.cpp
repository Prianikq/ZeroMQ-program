/* Исходный код программы потребителя (БСКИ), который получает информацию о координате и высоте от программ производителей (НО и РВ) и выводит их в терминал.
      Разработчик: Ляшун Д. С. */

#include "interface.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

int main (int32_t p_l_argc, const char *p_p_argv[]) {
    using namespace std::chrono_literals;
    const int32_t NUMBER_COMMAND_LINE_ARGUMENTS = 5; // Требуемое число аргументов в командной строке для запуска программы  
    
    std::string s_subscriber_name; // Имя потребителя
    int32_t l_wait_time; // Время ожидания потребителя между получением сообщений (в микросекундах)
    short n_control_port; // Номер порта ИУП
    std::string s_address; // Адрес, где находятся процессы-производители
    
    if (p_l_argc != NUMBER_COMMAND_LINE_ARGUMENTS) {
        std::cerr << "Ошибка! Произошел запуск программы производителя с некорректными аргументами командной строки!" << std::endl;
        return -1;
    }
    else {
        s_subscriber_name = std::string(p_p_argv[1]);
        s_address = std::string(p_p_argv[2]);
        l_wait_time = atoi(p_p_argv[3]);
        n_control_port = atoi(p_p_argv[4]);
    }
    
    zmq::context_t context(1); // Создание контекста (процесса, который управляет работой сокетов в рамках программы)
    
    // Создание сокета-подписчика и связывание его с адресами публикаторов
    zmq::socket_t coord_sub(context, ZMQ_SUB);
    coord_sub.connect("tcp://" + s_address + ":5561");
    coord_sub.set(zmq::sockopt::conflate, true);
    coord_sub.set(zmq::sockopt::rcvtimeo, 0);
    coord_sub.set(zmq::sockopt::subscribe, ""); // Устанавливает подписку - префикс данных сообщений, которые необходимо отбирать
    
    zmq::socket_t altit_sub(context, ZMQ_SUB);
    altit_sub.connect("tcp://" + s_address + ":5562");
    altit_sub.set(zmq::sockopt::conflate, true);
    altit_sub.set(zmq::sockopt::rcvtimeo, 0);
    altit_sub.set(zmq::sockopt::subscribe, ""); 
    
    zmq::socket_t radar_sub(context, ZMQ_SUB);
    radar_sub.connect("tcp://" + s_address + ":5563");
    radar_sub.set(zmq::sockopt::conflate, true);
    radar_sub.set(zmq::sockopt::rcvtimeo, 0);
    radar_sub.set(zmq::sockopt::subscribe, "");
    
    zmq::socket_t control_sub(context, ZMQ_SUB);
    control_sub.connect("tcp://" + s_address + ":" + std::to_string(n_control_port));
    control_sub.set(zmq::sockopt::conflate, true);
    control_sub.set(zmq::sockopt::rcvtimeo, 0);
    control_sub.set(zmq::sockopt::subscribe, "");

    //  Создание сокета для предварительной связи с публикаторами с целью синхронизации потока данных
    zmq::socket_t sub_sync(context, ZMQ_REQ);
    sub_sync.connect("tcp://" + s_address + ":5565");
    std::cout << "[ " << s_subscriber_name << " ]: Начинаю устанавливать связь с публикаторами." << std::endl;
    nZMQInterface::send_data(sub_sync, "");
    nZMQInterface::recieve_message(sub_sync);
    std::cout << "[ " << s_subscriber_name << " ]: Связь с публикаторами успешно установлена." << std::endl;
    
    //  Процесс получения данных от публикаторов
    bool b_last_coordinate_data = false, b_last_altitude_data = false, b_last_radar_data = false;
    int32_t l_x = 0, l_y = 0, l_altitude = 0, l_x_enemy = 0, l_y_enemy = 0;
    while (!b_last_coordinate_data || !b_last_altitude_data || !b_last_radar_data) {
        if (!b_last_coordinate_data) {
            auto res = nZMQInterface::recieve_data(coord_sub);
            if (res.has_value()) {
                nZMQInterface::rData data = res.value();
                if (data.m_e_type == nZMQInterface::eDataType::COORDINATE_END) {
                    b_last_coordinate_data = true;
                }
                else {
                    l_x = data.m_l_x_coord;
                    l_y = data.m_l_y_coord;
                }
            }
        }
        if (!b_last_altitude_data) {
            auto res = nZMQInterface::recieve_data(altit_sub);
            if (res.has_value()) {
                nZMQInterface::rData data = res.value();
                if (data.m_e_type == nZMQInterface::eDataType::ALTITUDE_END) {
                    b_last_altitude_data = true;
                }
                else {
                    l_altitude = data.m_l_altitude;
                }
            }
        }
        if (!b_last_radar_data) {
            auto res = nZMQInterface::recieve_data(radar_sub);
            if (res.has_value()) {
                nZMQInterface::rData data = res.value();
                if (data.m_e_type == nZMQInterface::eDataType::RADAR_END) {
                    b_last_radar_data = true;
                }
                else {
                    l_x_enemy = data.m_l_x_enemy;
                    l_y_enemy = data.m_l_y_enemy;
                }
            }
        }
        auto res = nZMQInterface::recieve_data(control_sub);
        if (res.has_value()) {
            nZMQInterface::rData data = res.value();
            if (data.m_e_type == nZMQInterface::eDataType::CONTROL) {
                std::cout << "Получил сообщение от ИУП: " << data.m_l_num << std::endl;
            }
        }
        std::cout << "[ " << s_subscriber_name << " ]: Имею данные X = " << l_x << ", Y = " << l_y << ", H = " << l_altitude << ", X_e = " << l_x_enemy << ", Y_e = " << l_y_enemy << ".\n";
        std::this_thread::sleep_for(std::chrono::microseconds(l_wait_time));
    }
    std::this_thread::sleep_for(1000ms);  // Дать ZeroMQ время чтобы сбросить вывод
    return 0;
}
