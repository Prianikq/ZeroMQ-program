/* Исходный код программы потребителя (БСКИ), который получает информацию о координате и высоте от программ производителей (НО и РВ) и выводит их в терминал.
      Разработчик: Ляшун Д. С. */

#include "interface.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

int main (int32_t p_l_argc, const char *p_p_argv[]) {
    using namespace std::chrono_literals;
    const int32_t NUMBER_COMMAND_LINE_ARGUMENTS = 2; // Требуемое число аргументов в командной строке для запуска программы  
    const std::string SUBSCRIBER_NAME = "БСКИ"; // Имя потребителя
    //const int32_t SUBSCRIBER_SLEEP_TIME = 10000; // Время ожидания потребителя между получением сообщений (в микросекундах)
    
    int32_t l_wait_time;
    
    if (p_l_argc != NUMBER_COMMAND_LINE_ARGUMENTS) {
        std::cerr << "Ошибка! Произошел запуск программы производителя с некорректными аргументами командной строки!" << std::endl;
        return -1;
    }
    else {
        l_wait_time = atoi(p_p_argv[1]);
    }
    
    zmq::context_t context(1); // Создание контекста (процесса, который управляет работой сокетов в рамках программы)
    
    // Создание сокета-подписчика и связывание его с адресами публикаторов
    zmq::socket_t subscriber (context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5561");
    subscriber.connect("tcp://localhost:5562");
    subscriber.set(zmq::sockopt::rcvhwm, 0); // Устанавливает неограниченное число хранимых сообщений в очереди получателя
    subscriber.set(zmq::sockopt::subscribe, ""); // Устанавливает подписку - префикс данных сообщений, которые необходимо отбирать

    //  Создание сокета для предварительной связи с публикаторами с целью синхронизации потока данных
    zmq::socket_t sync_coord_gen(context, ZMQ_REQ);
    zmq::socket_t sync_altit_gen(context, ZMQ_REQ);
    
    std::cout << "[ " << SUBSCRIBER_NAME << " ]: Начинаю устанавливать связь с публикаторами." << std::endl;
    sync_coord_gen.connect("tcp://localhost:5563");
    nZMQInterface::send_data(sync_coord_gen, "");
    nZMQInterface::recieve_message(sync_coord_gen);
    
    sync_altit_gen.connect("tcp://localhost:5564");
    nZMQInterface::send_data(sync_altit_gen, "");
    nZMQInterface::recieve_message(sync_altit_gen);
    std::cout << "[ " << SUBSCRIBER_NAME << " ]: Связь с публикаторами успешно установлена." << std::endl;
    
    
    //  Процесс получения данных от публикаторов
    bool b_last_coordinate_data = false, b_last_altitude_data = false;
    while (!b_last_coordinate_data || !b_last_altitude_data) {
        auto res = nZMQInterface::recieve_data(subscriber);
        if (res.has_value()) {
            nZMQInterface::rData data = res.value();
            switch (data.m_e_type) {
                case nZMQInterface::eDataType::ALTITUDE_END:
                    std::cout << "[ " << SUBSCRIBER_NAME << " ]: Получил последние данные о высоте." << std::endl;
                    b_last_altitude_data = true;
                    break;
                case nZMQInterface::eDataType::COORDINATE_END:
                    std::cout << "[ " << SUBSCRIBER_NAME << " ]: Получил последние данные о координате." << std::endl;
                    b_last_coordinate_data = true;
                    break;
                case nZMQInterface::eDataType::ALTITUDE:
                    std::cout << "[ " << SUBSCRIBER_NAME << " ]: Получил высоту H = " << data.m_l_altitude << "." << std::endl;
                    break;
                case nZMQInterface::eDataType::COORDINATE:
                    std::cout << "[ " << SUBSCRIBER_NAME << " ]: Получил координаты X = " << data.m_l_x_coord << ", Y = " << data.m_l_y_coord << "." << std::endl;
                    break;
            }
            std::this_thread::sleep_for(std::chrono::microseconds(l_wait_time));
        }
        else { 
            std::cerr << "Произошла ошибка при попытке получения сообщения со стороны потребителя!" << std::endl;
            return -2;
        }
    }
    std::this_thread::sleep_for(1000ms);  // Дать ZeroMQ время чтобы сбросить вывод
    return 0;
}
